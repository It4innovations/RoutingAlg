#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "Data.h"
#include "MCSimulation.h"
#include <omp.h>

void printHelp() {
    std::cout
            << "Usage: ptdr -c [number of runs] -n [number of samples] -e [edges_file.csv] -s [segment_tmc.csv] -p [profile.csv]"
            << std::endl;
}


int main(int argc, char *argv[]) {
    if (argc != 11) {
        printHelp();
        std::exit(1);
    }

    char **largv = argv;
    std::string edgesPath, segmentPath, profilePath;
    int samples = 0, runs = 0;
    while (*++largv) {
        switch ((*largv)[1]) {
            case 'e':
                edgesPath = *++largv;
                break;
            case 's':
                segmentPath = *++largv;
                break;
            case 'p':
                profilePath = *++largv;
                break;
            case 'n':
                samples = std::stoi(*++largv);
                break;
            case 'c':
                runs = std::stoi(*++largv);
                break;
            default:
                printHelp();
                std::exit(1);
        }
    }

    std::cout << "Edges: " << edgesPath << std::endl;
    std::cout << "Segments: " << segmentPath << std::endl;
    std::cout << "Profiles: " << profilePath << std::endl;

    std::cout << "Samples: " << samples << std::endl;
    std::cout << "Runs: " << runs << std::endl;


    // Load data
    std::cout << "Loading data...";
    std::cout.flush();
    auto startTime = std::chrono::high_resolution_clock::now();
    Routing::MCSimulation mc(edgesPath, profilePath);
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime).count();
    std::cout << elapsed << " ms" << std::endl;


    std::vector<float> result;
    //Single simulation
    // std::cout << "Running single simulation: " << std::flush;
    // startTime = std::chrono::high_resolution_clock::now();
    // result = Routing::MCSimulation::RunMonteCarloSimulation(segments, samples, 0, 7, 05, secondInterval);
    // elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
    // std::cout << elapsed << " ms" << std::endl;
    // Routing::Data::WriteResultSingle(result, "times_single.csv", secondInterval);
    // //std::cout << result[0] << std::endl;

    // All simulations
    std::cout << "Running all simulations: " << std::flush;
    startTime = std::chrono::high_resolution_clock::now();
    result = mc.RunMonteCarloSimulation(samples, 0, 7, 05, false);
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime).count();
    std::cout << elapsed << " ms" << std::endl;
    Routing::Data::WriteResultAll(result, "times_all.csv", samples, mc.GetSecondInterval());
    std::cout << result[0] << std::endl;



/**
  Scalability
**/
    //All simulations
    //  std::ofstream times_file("scalability.csv");
    //  times_file << "samples,threads,time" << std::endl;
    // std::cout << "Running all simulations" << std::endl;
    //  for (int i = 122; i <= 122; i += 2)
    //  {
    //    int threads = i < 1 ? 1 : i;
    //    omp_set_num_threads(threads);
    //    std::cout << threads << " threads" << std::endl;

    //    std::vector<long> times(runs);
    //    std::vector<int> results(runs);
    //    for(int r = 0; r < runs; r++)
    //    {
    //      std::cout << r << std::flush;
    //      auto startTime = std::chrono::high_resolution_clock::now();
    //      results[r]  = Routing::MCSimulation::RunMonteCarloAll(segments, samples)[0];
    //      times[r] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
    //      // Routing::Data::WriteResultAll(result, "times_all.csv", segments, samples);
    //    }

    //    std::cout << std::endl;


    //    int sum = 0;
    //    for (int r = 0; r < runs; ++r)
    //    {
    //      std::cout << "Run " << r << " " << times[r] << " ms " << " Time " << results[r] << " s" << std::endl;
    //      sum += times[r];
    //    }

    //    float average = (sum/runs);
    //    times_file << samples << "," << threads << "," << average << std::endl;

    //    std::cout << "Avg.: " << average << " ms" << std::endl;
    //  }
    //  times_file.close();

/**
  Multiple runs
**/
    // All simulations
    //  std::ofstream times_file("multiple_runs.csv", std::ofstream::app);
    // // times_file << "samples,threads,runs,first_time,avg_time" << std::endl;
    //  std::cout << "Running all simulations" << std::endl;


    //  std::cout << omp_get_max_threads() << " threads" << std::endl;

    //  std::vector<long> times(runs);
    //  std::vector<int> results(runs);
    //  for(int r = 0; r < runs; r++)
    //  {
    //    std::cout << r << std::flush;
    //    auto startTime = std::chrono::high_resolution_clock::now();
    //    results[r]  = mc.RunMonteCarloSimulation(samples, 0, 7, 5, false)[0];
    //    times[r] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
    //    // Routing::Data::WriteResultAll(result, "times_all.csv", segments, samples);
    //  }

    //  std::cout << std::endl;


    //  int sum = 0;
    //  for (int r = 0; r < runs; ++r)
    //  {
    //    std::cout << "Run " << r << " " << times[r] << " ms " << " Time " << results[r] << " s" << std::endl;
    //    sum += times[r];
    //  }

    //  float average = (sum/runs);
    //  times_file << samples << "," << omp_get_max_threads() << "," << runs << "," << times[0] << "," << average << std::endl;

    //  std::cout << "Avg.: " << average << " ms" << std::endl;
    //  times_file.close();


/**
  Ratio measurement
**/
    // auto totalStartTime = std::chrono::high_resolution_clock::now();

    // std::vector<long> times(runs);
    // std::vector<int> results(runs);
    // #pragma omp parallel for
    // for(int i = 0; i < runs; i++)
    // {
    //   std::cout << i << std::flush;
    //   auto startTime = std::chrono::high_resolution_clock::now();
    //   std::vector<float> result = Routing::MCSimulation::RunMonteCarloAll(segments, samples);
    //   times[i] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
    //   // Routing::Data::WriteResultAll(result, "times_all.csv", segments, samples);
    //   results[i] = result[0];
    // }

    // std::cout << std::endl;

    // long totalElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - totalStartTime).count();

    // int sum = 0;
    // for (int i = 0; i < runs; ++i)
    // {
    //   std::cout << "Run " << i << " " << times[i] << " ms " << " Time " << results[i] << " s" << std::endl;
    //   sum += times[i];
    // }
    // float average = (sum/runs);
    // std::cout << "Avg.: " << average << " ms" << std::endl;
    // std::cout << "Total: " << totalElapsed << " ms" << std::endl;
    // std::cout << "Ratio: " << (float)totalElapsed/(sum/runs) << std::endl;



    return 0;

}
