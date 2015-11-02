package ru.sbt.bit.ood.solid.homework.info.send;

/**
 * Created by maksim on 02/11/15.
 */
public interface SalaryInfoSender {
    void send(byte[] data, String recipients);
}
