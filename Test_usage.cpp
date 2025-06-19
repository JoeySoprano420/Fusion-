FPPCompletion comp;
comp.learnFromFile("main.fpp");

auto results = comp.suggest("pr");
for (auto& s : results)
    std::cout << "Suggest: " << s << "\n";
