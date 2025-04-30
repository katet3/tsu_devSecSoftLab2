#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <string>
#include "./lib/Allocator/Allocator.h"
#include "NodeBoolTree.h"
#include "boolinterval.h"
#include "boolequation.h"
#include "BBV.h"

// Аллокаторы для каждого типа
Allocator boolIntervalAlloc(sizeof(BoolInterval), 0, nullptr, "BoolIntervalAlloc");
Allocator boolEquationAlloc(sizeof(BoolEquation), 0, nullptr, "BoolEquationAlloc");
Allocator nodeBoolTreeAlloc(sizeof(NodeBoolTree), 0, nullptr, "NodeBoolTreeAlloc");

// Функция для чтения файла без Qt
std::vector<std::string> readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("File not found: " + filename);
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    file.close();
    return lines;
}

// Тест производительности: аллокатор vs new/delete
void performanceTest() {
    const int iterations = 10000;

    // Тест с аллокатором
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        void* mem = boolIntervalAlloc.Allocate(sizeof(BoolInterval));
        BoolInterval* interval = new (mem) BoolInterval("0101");
        interval->~BoolInterval();
        boolIntervalAlloc.Deallocate(mem);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto allocator_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Тест с new/delete
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        BoolInterval* interval = new BoolInterval("0101");
        delete interval;
    }
    end = std::chrono::high_resolution_clock::now();
    auto new_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "Performance results:\n";
    std::cout << "Allocator: " << allocator_time << " μs\n";
    std::cout << "New/Delete: " << new_time << " μs\n";
    std::cout << "Speedup: " << (double)new_time / allocator_time << "x\n\n";
}

// Тест обработки исключений
void exceptionTest() {
    try {
        // Искусственно исчерпываем память
        std::cout << "Testing out-of-memory...\n";
        while (true) {
            void* ptr = boolIntervalAlloc.Allocate(sizeof(BoolInterval));
            if (!ptr) throw std::bad_alloc();
            // Не вызываем деструктор специально для теста
        }
    } catch (const std::bad_alloc& e) {
        std::cerr << "Caught exception: " << e.what() << "\n";
    }

    try {
        // Тест двойного освобождения (для Valgrind/ASAN)
        std::cout << "Testing double-free...\n";
        void* mem = boolIntervalAlloc.Allocate(sizeof(BoolInterval));
        BoolInterval* interval = new (mem) BoolInterval("0101");
        boolIntervalAlloc.Deallocate(mem);
        boolIntervalAlloc.Deallocate(mem);  // Intentional error
    } catch (...) {
        std::cerr << "Caught double-free attempt\n";
    }
}

int main() {
    // Тест производительности
    performanceTest();

    // Тест исключений
    exceptionTest();

    // Основная логика программы
    try {
        // Чтение входного файла
        auto lines = readFile("Sat_ex30_3.pla");
        if (lines.empty()) {
            throw std::runtime_error("Input file is empty");
        }

        // Создание CNF
        int cnfSize = lines.size();
        BoolInterval** CNF = new BoolInterval*[cnfSize];
        for (int i = 0; i < cnfSize; ++i) {
            CNF[i] = new BoolInterval(lines[i].c_str());
        }

        // Создание корневого интервала
        std::string rootvec(lines[0].size(), '0');
        std::string rootdnc(lines[0].size(), '1');
        BoolInterval* root = new BoolInterval(rootvec.c_str(), rootdnc.c_str());

        // Создание уравнения
        BoolEquation* equation = new BoolEquation(CNF, root, cnfSize, cnfSize, BBV(rootvec.c_str()));

        // Алгоритм поиска (упрощенный пример)
        std::stack<NodeBoolTree*> boolTreeStack;
        
        // Использование аллокатора для NodeBoolTree
        void* nodeMem = nodeBoolTreeAlloc.Allocate(sizeof(NodeBoolTree));
        NodeBoolTree* startNode = new (nodeMem) NodeBoolTree(equation);
        boolTreeStack.push(startNode);

        while (!boolTreeStack.empty()) {
            NodeBoolTree* current = boolTreeStack.top();
            boolTreeStack.pop();

            // Упрощенная логика обработки
            if (current->eq->CheckRules() == 1) {
                std::cout << "Solution found!\n";
                break;
            }

            // Пример ветвления
            int branchCol = current->eq->ChooseColForBranching();
            
            // Использование аллокатора для BoolEquation
            void* eq0Mem = boolEquationAlloc.Allocate(sizeof(BoolEquation));
            BoolEquation* eq0 = new (eq0Mem) BoolEquation(*current->eq);
            void* eq1Mem = boolEquationAlloc.Allocate(sizeof(BoolEquation));
            BoolEquation* eq1 = new (eq1Mem) BoolEquation(*current->eq);
            
            eq0->Simplify(branchCol, '0');
            eq1->Simplify(branchCol, '1');

            void* node1Mem = nodeBoolTreeAlloc.Allocate(sizeof(NodeBoolTree));
            NodeBoolTree* node1 = new (node1Mem) NodeBoolTree(eq1);
            void* node0Mem = nodeBoolTreeAlloc.Allocate(sizeof(NodeBoolTree));
            NodeBoolTree* node0 = new (node0Mem) NodeBoolTree(eq0);

            boolTreeStack.push(node1);
            boolTreeStack.push(node0);

            // Освобождение памяти
            current->~NodeBoolTree();
            nodeBoolTreeAlloc.Deallocate(current);
        }

        // Очистка
        for (int i = 0; i < cnfSize; ++i) {
            delete CNF[i];
        }
        delete[] CNF;
        delete root;
        delete equation;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}