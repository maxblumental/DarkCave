package ru.sbt.bit.ood.solid.homework;

import org.springframework.mail.javamail.JavaMailSenderImpl;
import org.springframework.mail.javamail.MimeMessageHelper;
import ru.sbt.bit.ood.solid.homework.info.format.HtmlSalaryInfoFormatter;
import ru.sbt.bit.ood.solid.homework.info.process.TotalSum;
import ru.sbt.bit.ood.solid.homework.info.retrieve.SQLSalaryInfoRetriever;
import ru.sbt.bit.ood.solid.homework.info.send.EmailSalaryInfoSender;

import javax.mail.MessagingException;
import javax.mail.internet.MimeMessage;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.time.LocalDate;

public class SalaryHtmlReportNotifier2 {

    private SalaryReportNotifier notifier;

    private Connection connection;

    public SalaryHtmlReportNotifier2(Connection databaseConnection) {
        this.connection = databaseConnection;
        notifier = new SalaryReportNotifier(new SQLSalaryInfoRetriever(connection), new HtmlSalaryInfoFormatter(),
                new TotalSum(), new EmailSalaryInfoSender());
    }

    public void generateAndSendHtmlSalaryReport(String departmentId, LocalDate dateFrom, LocalDate dateTo, String recipients) {
        notifier.sendReport(departmentId, dateFrom, dateTo, recipients);
    }
}
