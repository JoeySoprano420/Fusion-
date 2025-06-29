g++ fusionpp.cpp -o fusionpp
./fusionpp hello.fpp
./game.exe

g++ -o fusionpp fusionpp_massive_core.cpp -lssl -lcrypto
./fusionpp

g++ -o fusionpp main.cpp -lssl -lcrypto
./fusionpp

g++ -o fusion_compiler fpp_compiler_main.cpp -lssl -lcrypto
./fusion_compiler
./example.exe

g++ -o fusionpp fusion_compile_main.cpp -lssl -lcrypto
./fusionpp
./main.exe

g++ main_runtime.cpp -std=c++17 -o fusion_live
./fusion_live

python gpt_finetune_trainer.py
# 🔁 Then fine-tune via OpenAI API or load into prompt history
