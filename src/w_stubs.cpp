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

#include "Budget.h"

#include "s_fileio.h"
#include "s_gen.h"
#include "s_sim.h"

#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"


#include <SDL2/SDL.h>

#include <algorithm>
#include <string>

/* Stubs */


int PunishCnt;
int autoBulldoze, autoBudget;
int autoGo;

int InitSimLoad;
int ScenarioID;
int NoDisasters;

std::string StartupName;

namespace
{
    int gameLevel{};

    std::string cityName{};
}


void GameLevel(const int level)
{
    gameLevel = std::clamp(level, 0, 2);
}


int GameLevel()
{
    return gameLevel;
}


void CityName(const std::string& name)
{
    cityName = name;
}


const std::string& CityName()
{
    return cityName;
}


int TickCount()
{
    return static_cast<int>(SDL_GetTicks());
}


void DoPlayNewCity(Budget& budget)
{
    Eval("UIPlayNewCity");

    GameLevel(0);
    CityName("NowHere");
    if (GameLevel() == -1) { GameLevel(0); }
    GenerateNewCity(budget);

    /*
    proc UIPlayGame {}
    {
        global State
        global Priority
        set State play
        sim Resume
        sim Speed 3
        sim AutoGoto 1
        SetPriority $Priority

    }
    */

    Resume();
    SimSpeed(SimulationSpeed::Normal);
}


void DoReallyStartGame()
{
    Eval("UIReallyStartGame");

    /*
    proc UIPickScenarioMode{}
    {
        global State
            global CityLibDir
            set State scenario
            sim Pause
            WithdrawAll
            InitHistory
            UIGenerateCityNow
            InitScenarios
            InitVotesForUseThisMap
            ShowScenarios
    }
    */
}


void DoStartLoad()
{
    Eval("UIStartLoad");
}


void DoStartScenario(int scenario)
{
    Eval("UIStartScenario " + std::to_string(scenario));
}


void DropFireBombs()
{
    Eval("DropFireBombs");
}


void InitGame()
{
    SimSpeed(SimulationSpeed::Paused);
}


void GameStarted(Budget& budget)
{
    switch (Startup)
    {
    case -2: /* Load a city */
        if (LoadCity(StartupName))
        {
            DoStartLoad();
            StartupName = "";
            break;
        }

    case -1:
        if (!StartupName.empty())
        {
            CityName(StartupName);
            StartupName = "";
        }
        else
        {
            CityName("NowHere");
        }
        DoPlayNewCity(budget);
        break;

    case 0:
        DoReallyStartGame();
        break;

    default: /* scenario number */
        DoStartScenario(Startup);
        break;
    }
}
