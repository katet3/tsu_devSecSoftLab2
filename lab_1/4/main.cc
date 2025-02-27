#include <iostream>

class AbstractEntity {
public:
    virtual ~AbstractEntity() = default;

    virtual void PrintInfo() const = 0;
};

class AccountEntity : public AbstractEntity {
public:
    void PrintInfo() const override {
        std::cout << "This is an AccountEntity." << std::endl;
    }
};

class RoleEntity : public AbstractEntity {
public:
    void PrintInfo() const override {
        std::cout << "This is a RoleEntity." << std::endl;
    }
};

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual void SaveEntity(const AbstractEntity& entity) = 0;
};

class AccountRepository : public IRepository {
public:
    void SaveEntity(const AbstractEntity& entity) override {
        std::cout << "Saving AccountEntity..." << std::endl;
        entity.PrintInfo(); // Выполняем специфические действия для AccountEntity
    }
};

class RoleRepository : public IRepository {
public:
    void SaveEntity(const AbstractEntity& entity) override {
        std::cout << "Saving RoleEntity..." << std::endl;
        entity.PrintInfo();
    }
};

class RepositoryFactory {
public:
    static IRepository* CreateRepository(const AbstractEntity& entity) {
        if (dynamic_cast<const AccountEntity*>(&entity)) {
            return new AccountRepository();
        } else if (dynamic_cast<const RoleEntity*>(&entity)) {
            return new RoleRepository();
        }
        throw std::runtime_error("Unsupported entity type");
    }
};

int main() {
    AccountEntity account_entity;
    RoleEntity role_entity;

    IRepository* ptr_account_repository = RepositoryFactory::CreateRepository(account_entity);
    IRepository* ptr_role_repository = RepositoryFactory::CreateRepository(role_entity);

    ptr_account_repository->SaveEntity(account_entity);
    ptr_role_repository->SaveEntity(role_entity);

    delete ptr_account_repository;
    delete ptr_role_repository;

    return 0;
}