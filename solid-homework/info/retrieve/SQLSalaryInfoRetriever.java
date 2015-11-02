package ru.sbt.bit.ood.solid.homework.info.retrieve;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

public class SQLSalaryInfoRetriever implements SalaryInfoRetriever {

    private Connection connection;

    public SQLSalaryInfoRetriever(Connection databaseConnection) {
        this.connection = databaseConnection;
    }

    public List<EmployeeSalaryInfo> retrieve(String departmentId, LocalDate dateFrom, LocalDate dateTo) {
        List<EmployeeSalaryInfo> departmentInfo = new ArrayList<EmployeeSalaryInfo>();

        try {
            // prepare statement with sql
            PreparedStatement ps = connection.prepareStatement("select emp.id as emp_id, emp.name as amp_name, sum(salary) as salary from employee emp left join" +
                    "salary_payments sp on emp.id = sp.employee_id where emp.department_id = ? and" +
                    " sp.date >= ? and sp.date <= ? group by emp.id, emp.name");

            // inject parameters to sql
            ps.setString(0, departmentId);
            ps.setDate(1, new java.sql.Date(dateFrom.toEpochDay()));
            ps.setDate(2, new java.sql.Date(dateTo.toEpochDay()));
            // execute query and get the results
            ResultSet results = ps.executeQuery();

            while (results.next()) {
                EmployeeSalaryInfo empInfo = new EmployeeSalaryInfo(results.getString("emp_name"),
                        results.getDouble("salary"));
                results.getDouble("salary");
                departmentInfo.add(empInfo);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return departmentInfo;
    }
}
