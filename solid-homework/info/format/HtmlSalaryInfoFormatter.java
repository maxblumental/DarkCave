package ru.sbt.bit.ood.solid.homework.info.format;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;
import ru.sbt.bit.ood.solid.homework.info.process.SalaryInfoProcessor;

import java.util.List;

/**
 * Created by maksim on 02/11/15.
 */
public class HtmlSalaryInfoFormatter implements SalaryInfoFormatter {

    public byte[] format(List<EmployeeSalaryInfo> departmentInfo, SalaryInfoProcessor processor) {
        // create a StringBuilder holding a resulting html
        StringBuilder resultingHtml = new StringBuilder();
        resultingHtml.append("<html><body><table><tr><td>Employee</td><td>Salary</td></tr>");
        double totals = 0;
        for (EmployeeSalaryInfo empInfo : departmentInfo) {
            // process each row of query results
            resultingHtml.append("<tr>"); // add row start tag
            resultingHtml.append("<td>").append(empInfo.getName()).append("</td>"); // appending employee name
            resultingHtml.append("<td>").append(empInfo.getSalary()).append("</td>"); // appending employee salary for period
            resultingHtml.append("</tr>"); // add row end tag
        }
        resultingHtml.append("<tr><td>Total</td><td>")
                .append(processor.process(departmentInfo))
                .append("</td></tr>");
        resultingHtml.append("</table></body></html>");

        return resultingHtml.toString().getBytes();
    }
}
