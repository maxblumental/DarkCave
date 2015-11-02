package ru.sbt.bit.ood.solid.homework.info;

public class EmployeeSalaryInfo {
    private String name;
    private Double salary;

    public EmployeeSalaryInfo(String name, Double salary) {
        this.name = name;
        this.salary = salary;
    }

    public String getName() {
        return name;
    }

    public Double getSalary() {
        return salary;
    }
}
