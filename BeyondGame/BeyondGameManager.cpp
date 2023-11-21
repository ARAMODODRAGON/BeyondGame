#include "BeyondGameManager.hpp"

BeyondGameManager::BeyondGameManager() { }

BeyondGameManager::~BeyondGameManager() { }

void BeyondGameManager::on_init() { 
	BE_LOG("init");
}

void BeyondGameManager::on_step() { 
	BE_LOG("step");
}

void BeyondGameManager::on_draw() { 
	BE_LOG("draw");
}

void BeyondGameManager::on_exit() { 
	BE_LOG("exit");
}
