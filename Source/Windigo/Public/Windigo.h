// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

//General Log
DECLARE_LOG_CATEGORY_EXTERN(WindigoLog, Log, All);

//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(WindigoInit, Log, All);

//Singleton log
DECLARE_LOG_CATEGORY_EXTERN(WindigoSingleton, Log, All);

//Logging for Windigo AI system
DECLARE_LOG_CATEGORY_EXTERN(WindigoAI, Log, All);

//Critical Errors
DECLARE_LOG_CATEGORY_EXTERN(WindigoCriticalError, Log, All);