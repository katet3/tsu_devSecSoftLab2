#include <iostream>

class Validator {
public:
    virtual ~Validator() = default;

    virtual bool Validate(const class Product& product) const = 0;
};

class Product {
private:
    double price_;

public:
    explicit Product(double price)
        : price_(price) {}

    double GetPrice() const {
        return price_;
    }

    bool IsValid(const Validator& validator) const;
};

bool Product::IsValid(const Validator& validator) const {
    return validator.Validate(*this);
}

class MainValidator : public Validator {
public:
    bool Validate(const Product& product) const override {
        return product.GetPrice() > 0;
    }
};

class CustomerServiceValidator : public Validator {
public:
    bool Validate(const Product& product) const override {
        return product.GetPrice() > 1000;
    }
};

int main() {
    Product product_one(500);
    Product product_two(1500);

    MainValidator main_validator;
    CustomerServiceValidator customer_service_validator;

    {
        std::cout << "ProductOne is valid for MainValidator: "
        << (product_one.IsValid(main_validator) ? "true" : "false") << std::endl;
        
        std::cout << "ProductOne is valid for CustomerServiceValidator: "
        << (product_one.IsValid(customer_service_validator) ? "true" : "false") << std::endl;
        
        std::cout << "ProductTwo is valid for MainValidator: "
        << (product_two.IsValid(main_validator) ? "true" : "false") << std::endl;
        
        std::cout << "ProductTwo is valid for CustomerServiceValidator: "
        << (product_two.IsValid(customer_service_validator) ? "true" : "false") << std::endl;
    }

    return 0;
}