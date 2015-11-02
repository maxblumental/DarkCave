package ru.sbt.bit.ood.solid.homework.info.format;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;
import ru.sbt.bit.ood.solid.homework.info.process.SalaryInfoProcessor;

import java.util.List;

public interface SalaryInfoFormatter {
    byte[] format(List<EmployeeSalaryInfo> departmentInfo, SalaryInfoProcessor processor);
}
