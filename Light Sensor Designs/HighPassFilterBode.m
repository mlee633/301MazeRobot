% HighPassFilterBode.m
% Author: Oliver Wright

clc;
clear;
close all;

%% fb = 120Hz design
f = 10;
T = 1/(2 * pi * f);
sys = tf([T, 0], [T, 1]);

figure;
bode(sys);
title(f + "Hz High Pass Filter")
grid;


