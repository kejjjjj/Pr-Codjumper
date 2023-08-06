#pragma once

#include "pch.hpp"

class EvarBase {
public:
    virtual ~EvarBase() = default;
    virtual std::string get_name() const = 0;
};

template<typename T>
class Evar : public EvarBase {
public:
    Evar(const std::string& name) : name(name), data(nullptr) {}

    Evar(const std::string& name, const T& value) : name(name), data(new T(value)) {}

    Evar(const std::string& name, const T* array, size_t size) : name(name), data(new T[size]) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = array[i];
        }
    }

    ~Evar() = default;

    T& get() const noexcept {
        if (!data) {
            FatalError("Evar is not initialized");
        }
        return *data.get();
    }

    T* get_array() const noexcept {
        if (!data) {
            FatalError("Evar is not initialized");
        }
        return data.get();
    }

    std::string& get_name() const override {
        return name;
    }

private:
    std::string name;
    std::unique_ptr<T[]> data;
};

class EvarTable {
public:
    static EvarTable& getInstance() {
        static EvarTable instance;
        return instance;
    }

    template<typename T>
    void add_variable(const std::string& name, const T& value) {
        variables.push_back(std::make_unique<Evar<T>>(name, value));
    }

    template<typename T>
    void add_array(const std::string& name, const T* array, const size_t size) {
        variables.push_back(std::make_unique<Evar<T>>(name, array, size));
    }

    template<typename T>
    const Evar<T>* find(const std::string& name) const {
        for (const auto& var : variables) {
            const Evar<T>* dynVar = dynamic_cast<const Evar<T>*>(var.get());
            if (dynVar && dynVar->get_name().compare(name) == false) {
                return dynVar;
            }
        }
        return nullptr;
    }

private:
    EvarTable() = default;
    ~EvarTable() = default;
    EvarTable(const EvarTable&) = delete;
    EvarTable& operator=(const EvarTable&) = delete;

    std::list<std::unique_ptr<EvarBase>> variables;
};