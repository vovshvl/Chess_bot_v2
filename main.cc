#include "UCIHandler.hh"
int Benchmark();
int minmaxBenchmark();
int game();

int main() {
    //minmaxBenchmark();
    //game();
    UCIHandler uci;
    uci.loop();
    return 0;
}