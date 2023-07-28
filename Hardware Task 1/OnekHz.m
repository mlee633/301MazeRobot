clc;
clear;
close all;

PERIOD = 20e-6;

data = csvread("1khz.csv");
x = (0:(length(data) - 1)) * PERIOD;
plot(x, data);