package ru.sbt.bit.ood.solid.homework.info.process;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;
import java.util.List;

public interface SalaryInfoProcessor {
    Double process(List<EmployeeSalaryInfo> departmentInfo);
}
