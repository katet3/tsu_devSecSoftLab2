#ifndef BRANCHING_STRATEGY_H
#define BRANCHING_STRATEGY_H

#include "boolinterval.h"
#include <vector>

/*
    Абстрактный класс стратегии ветвления
    который содержит интерфейс стратегии
*/
class BranchingStrategy {
public:
    virtual ~BranchingStrategy() = default;
    
    virtual int chooseBranchingIndex(BoolInterval** cnf, int cnfSize, BBV& mask) const = 0;
};

/*
    Стратегия ветвления по столбцам
*/
class ColumnBranchingStrategy : public BranchingStrategy {
public:
    int chooseBranchingIndex(BoolInterval** cnf, int cnfSize, BBV& mask) const override;
};

/*
    Стратегия ветвления по строкам
    Maximum Occurrences in Minimum Clauses (MOM)
*/
class RowBranchingStrategy : public BranchingStrategy {
public:
    int chooseBranchingIndex(BoolInterval** cnf, int cnfSize, BBV& mask) const override;
};

#endif // BRANCHING_STRATEGY_H