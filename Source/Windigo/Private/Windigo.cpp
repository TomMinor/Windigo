// Fill out your copyright notice in the Description page of Project Settings.

#include "Windigo.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Windigo, "Windigo" );

//General Log
DEFINE_LOG_CATEGORY(WindigoLog);

//Logging during game startup
DEFINE_LOG_CATEGORY(WindigoInit);

//Singleton log
DEFINE_LOG_CATEGORY(WindigoSingleton);

//Logging for Windigo AI system
DEFINE_LOG_CATEGORY(WindigoAI);

//Critical Errors
DEFINE_LOG_CATEGORY(WindigoCriticalError);