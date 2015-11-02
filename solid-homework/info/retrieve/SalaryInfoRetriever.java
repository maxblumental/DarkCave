package ru.sbt.bit.ood.solid.homework.info.retrieve;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;

import java.time.LocalDate;
import java.util.List;

public interface SalaryInfoRetriever {
    List<EmployeeSalaryInfo> retrieve(String departmentId, LocalDate dateFrom, LocalDate dateTo);
}
