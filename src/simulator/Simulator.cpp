
//
// Created by thomas on 01/11/18.
//

#include "Simulator.hpp"
#include <BodyGenerators/SphereBodyGenerator.hpp>
#include <BodyGenerators/BinaryBodyGenerator.hpp>
#include <filesystem>
#include <fstream>
#include <thread>
#include <base/getConcreteUniverse.hpp>
#include <base/Exceptions.hpp>
#include <iostream>


Simulator::Simulator(Settings settings) : settings{std::move(settings)}, universe{getConcreteUniverse(settings)} {
    const std::map<std::string, std::function<std::unique_ptr<BodyGenerator>()>> bodyGeneratorOptions{
            {"sphere", [&]() { return std::make_unique<SphereBodyGenerator>(settings); }},
            {"binary", [&]() { return std::make_unique<BinaryBodyGenerator>(settings); }}
    };

    auto iter = bodyGeneratorOptions.find(settings.bodyGeneratorType);
    if (iter == bodyGeneratorOptions.end()) {
        std::stringstream ss;
        for (auto it = bodyGeneratorOptions.begin(); it != bodyGeneratorOptions.end(); ++it) {
            ss << (it == bodyGeneratorOptions.begin() ? "" : ",") << (*it).first;
        }
        throw InvalidOptionValue{"bodyGeneratorType", settings.bodyGeneratorType, ss.str()};
    }
    universe->init(iter->second());

    if (!std::filesystem::exists(settings.resultsDir)) {
        std::filesystem::create_directory(settings.resultsDir);
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    if (settings.enableFileOutput) {

        std::stringstream ss;
        ss << std::put_time(&tm, "%Y%m%d-%H%M%S");

        outputDir = settings.resultsDir + '/' + ss.str();

        std::filesystem::create_directory(outputDir);

        assert(settings.snapshotDelta > 0);
    }
}

void Simulator::snapshot(unsigned int fileNr) const {
    if (settings.enableFileOutput) {
        if (std::ofstream out{outputDir + '/' + settings.resultsFilenamePrefix + std::to_string(fileNr) + ".csv"};
                out.is_open()) {
            universe->logInternalState(out);
        }
        else {
            throw std::runtime_error{"could not write to file"};
        }
    }
}

void Simulator::run() {
//    unsigned step = 0;
    auto step = std::make_shared<unsigned>(0);

    unsigned progressWait = 5;
    unsigned lastStep = 0;
    unsigned timeBetweenUpdates = 0;
    unsigned expectedTimeUntilUpdate = progressWait;
    double lastTimeLeft = 0;

    auto printProgress = [&]() {
        unsigned secLeft;
        if (lastStep == *step) {
            timeBetweenUpdates += progressWait;
        } else {
            expectedTimeUntilUpdate = timeBetweenUpdates;
            timeBetweenUpdates = progressWait;
            lastTimeLeft = double(settings.nrOfSteps - *step) / (*step - lastStep);
        }
        secLeft = (unsigned)std::max((lastTimeLeft * expectedTimeUntilUpdate) + progressWait - timeBetweenUpdates, 0.0);

        std::cout << double(*step) * 100 / settings.nrOfSteps
                  << "% done (" << *step << "/" << settings.nrOfSteps;
        if (*step) {
            std::cout << "). About "<< secLeft / 60 << " min " << secLeft % 60 << " sec left.\n";
        } else {
            std::cout << "). About ? min ? sec left.\n";
        }
        lastStep = *step;
    };


    std::thread progress{[=]() {
        while (*step < settings.nrOfSteps) {
            printProgress();
            std::this_thread::sleep_for(std::chrono::seconds(progressWait));
        }
    }};
    progress.detach();

    auto start = std::chrono::steady_clock::now();

    snapshot(0);
    while (*step < settings.nrOfSteps - settings.nrOfSteps % settings.snapshotDelta) {
        universe->step(settings.snapshotDelta);
        *step += settings.snapshotDelta;
        snapshot(*step);
    }
    if (unsigned leftOver = settings.nrOfSteps % settings.snapshotDelta; leftOver) {
        universe->step(leftOver);
        snapshot(*step + leftOver);
    }

    printProgress();

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "total duration: " << diff.count() << "ms" << "\n";

}

