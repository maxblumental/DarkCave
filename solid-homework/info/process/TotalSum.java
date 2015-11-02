package ru.sbt.bit.ood.solid.homework.info.process;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;

import java.util.List;

/**
 * Created by maksim on 02/11/15.
 */
public class TotalSum implements SalaryInfoProcessor {
    public Double process(List<EmployeeSalaryInfo> departmentInfo) {
        double total = 0;
        for (EmployeeSalaryInfo empInfo : departmentInfo) {
            total += empInfo.getSalary();
        }

        return total;
    }
}
