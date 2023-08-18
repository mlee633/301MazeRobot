% LowPassFilterBode.m
% Author: Oliver Wright

clc;
clear;
close all;

%% fb = 12.06Hz design
f = 12.06;
T = 1/(2 * pi * f);
sys = tf(1, [T, 1]);

figure;
bode(sys);
title(f + "Hz Low Pass Filter")
grid;

% Attenuation @ 120Hz


%% fb = 60Hz design
f = 60;
T = 1/(2 * pi * f);
sys = tf(1, [T, 1]);

figure;
bode(sys);
title(f + "Hz Low Pass Filter")
grid;