package ru.sbt.bit.ood.solid.homework;

import ru.sbt.bit.ood.solid.homework.info.EmployeeSalaryInfo;
import ru.sbt.bit.ood.solid.homework.info.format.SalaryInfoFormatter;
import ru.sbt.bit.ood.solid.homework.info.process.SalaryInfoProcessor;
import ru.sbt.bit.ood.solid.homework.info.retrieve.SalaryInfoRetriever;
import ru.sbt.bit.ood.solid.homework.info.send.SalaryInfoSender;

import java.time.LocalDate;
import java.util.List;

public class SalaryReportNotifier {

    private SalaryInfoRetriever retriever;
    private SalaryInfoFormatter formatter;
    private SalaryInfoProcessor processor;
    private SalaryInfoSender sender;

    public SalaryReportNotifier(SalaryInfoRetriever retriever, SalaryInfoFormatter formatter,
                                SalaryInfoProcessor processor, SalaryInfoSender sender) {
        this.retriever = retriever;
        this.formatter = formatter;
        this.processor = processor;
        this.sender = sender;
    }

    public void sendReport(String departmentId, LocalDate dateFrom,
                           LocalDate dateTo, String recipients) {
        List<EmployeeSalaryInfo> departmentInfo = retriever.retrieve(departmentId, dateFrom, dateTo);
        byte[] data = formatter.format(departmentInfo, processor);
        sender.send(data, recipients);
    }
}
