#include "Application.h"
#include "imgui/imgui.h"
#include "classes/Chess.h"
#include "log.h"
#include <string>

namespace ClassGame {
        //
        // our global variables
        //
        Chess *game = nullptr;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            //Log::initalize(Log::WARNING);
            //Log::log(Log::INFO, "Setting up Chess Game and Board");
            game = new Chess();
            game->setUpBoard();
            gameWinner = -1;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
                
                ImGui::Begin("Settings");
                ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                ImGui::Text("Current Board State: %s", game->stateString().c_str());
                if (game->checkForDraw()) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Draw!");
                } else {
                    if (gameWinner != -1) {
                        ImGui::Text("Game Over!");
                        ImGui::Text("Winner: %d", gameWinner);
                    }
                }
                if (ImGui::Button("Reset Game")) {
                    game->stopGame();
                    game->setUpBoard();
                    game->getPlayerAt(0)->setAIPlayer(false);
                    game->getPlayerAt(1)->setAIPlayer(false);
                    gameWinner = -1;
                }
                if (ImGui::Button("Add AI")){
                    // if(game->getPlayerAt(1)->isAIPlayer()){
                    //     game->setAIPlayer(0);
                    // }
                    // else{
                        game->setAIPlayer(1);
                        game->getPlayerAt(1)->setAIPlayer(true);
                    // }                    
                }
                ImGui::End();

                if (game->gameHasAI() && game->getCurrentPlayer()->isAIPlayer())
                {
                    game->updateAI();
                }

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            //Log::log(Log::INFO, "Ending player's turn, checking for winner");
            Player *winner = game->checkForWinner();
            if (winner)
            {
                //Log::log(Log::INFO, "Found a winner, will inform the system now");
                gameWinner = winner->playerNumber();
            }
        }
}
