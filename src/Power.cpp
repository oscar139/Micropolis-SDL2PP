// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright � 2022 Leeor Dicker
//
// Portions Copyright � 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#include "main.h"

#include "Map.h"

#include "s_alloc.h"
#include "s_msg.h"

/* Power Scan */

namespace
{
    constexpr int CoalPowerProvided{ 700 };
    constexpr int NuclearPowerProvided{ 2000 };

    constexpr auto PowerMapRow = ((SimWidth + 15) / 16);
    constexpr auto PowerMapSize = (PowerMapRow * SimHeight);
    constexpr auto PowerStackSize = ((SimWidth * SimHeight) / 4);

    int PowerStackNum{};
    char PowerStackX[PowerStackSize]{}, PowerStackY[PowerStackSize]{};

    std::array<int, PowerMapSize> PowerMap{};
};


void ResetPowerStackCount()
{
    PowerStackNum = 0;
}


void ResetPowerMap()
{
    PowerMap.fill(0);
}


void SetPowerBit()
{
    /* XXX: assumes 120x100 */
    const auto powerWrd = (SimulationTarget.x / 16) + (SimulationTarget.y * 8);
    PowerMap[powerWrd] |= 1 << (SimulationTarget.x & 15);
}


bool PowerBitSet(const int x, const int y)
{
    const auto powerWord = (x / 16) + (y * 8);
    return (PowerMap[powerWord] & (1 << (x & 15))) != 0;
}


bool TestPowerBit(const int x, const int y)
{
    const auto tile = maskedTileValue(x, y);
    if ((tile == NUCLEAR) || (tile == POWERPLANT))
    {
        return true;
    }

    /* XXX: assumes 120x100 */
    int PowerWrd = (x / 16) + (y * 8);
    if (PowerWrd >= PowerMapSize)
    {
        return false;
    }

    return PowerBitSet(x, y);
}


bool TileIsConductive(int TFDir)
{
    Point<int> saved = SimulationTarget;

    if (MoveSimulationTarget(TFDir))
    {
        if ((Map[SimulationTarget.x][SimulationTarget.y] & CONDBIT) && (!TestPowerBit(SimulationTarget.x, SimulationTarget.y)))
        {
            SimulationTarget = saved;
            return true;
        }
    }

    SimulationTarget = saved;

    return false;
}


void PushPowerStack()
{
    if (PowerStackNum < (PowerStackSize - 2))
    {
        PowerStackNum++;
        PowerStackX[PowerStackNum] = SimulationTarget.x;
        PowerStackY[PowerStackNum] = SimulationTarget.y;
    }
}


void PullPowerStack()
{
    if (PowerStackNum > 0)
    {
        SimulationTarget = { PowerStackX[PowerStackNum], PowerStackY[PowerStackNum] };
        PowerStackNum--;
    }
}


void DoPowerScan()
{
    ResetPowerMap();

    int powerAvailable = (CoalPop * CoalPowerProvided) + (NuclearPop * NuclearPowerProvided);
    int powerConsumed = 0;

    int conductiveTileCount{};
    while (PowerStackNum)
    {
        PullPowerStack();
        
        int ADir{4};
        do
        {
            if (++powerConsumed > powerAvailable)
            {
                SendMes(NotificationId::BrownoutsReported);
                return;
            }
            
            MoveSimulationTarget(ADir);
            SetPowerBit();

            conductiveTileCount = 0;
            int searchDirection{0};
            while ((searchDirection < 4) && (conductiveTileCount < 2))
            {
                if (TileIsConductive(searchDirection))
                {
                    conductiveTileCount++;
                    ADir = searchDirection;
                }
                searchDirection++;
            }
            if (conductiveTileCount > 1)
            {
                PushPowerStack();
            }
        } while (conductiveTileCount);
    }
}
