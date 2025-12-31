clc
clear
close all

%% Dissipazione mosfet
% https://www.youtube.com/watch?v=GrvvkYTW_0k

% IRLZ44N 

temperaturaOperativa = 175; % [°C]
junctionToAmbient = 62; % [°C/W]

% mi tengo largo, considerando anche l'estate, prendo il caso peggiore
temperaturaAmbiente = 30;

potenzaDissipataMax = (temperaturaOperativa - temperaturaAmbiente) / junctionToAmbient

% ho considerato la resistenza a 10V, anche se gli metto 12V
resistenzaOn = 0.022;

% striscia led aliexpress
% sarebbe dai 16 ai 18 watt al metro, mi metto conservativo e dico 18

consumoAlMetro = 18; % [W/m]
lunghezza = 1; % [m]
voltaggio = 12; % [V] -> voltaggio striscia led

potenza = consumoAlMetro * lunghezza;

% inverto P = I*V, trovando I = P/V
intensitaCorrente = potenza / voltaggio;

% P = I*V, R * I^2
potenzaDissipata = resistenzaOn * intensitaCorrente^2


%% Calcolo corrente necessaria alimentazione

maxCorrenteESP = 20/1000; % [A]


consumoLedAlMetro = 18; % [W/m]

metri = 1;

consumo = consumoLedAlMetro * metri; % [W]

voltaggio = 12; %[v]

% W = v*i;
% i = w/v;

correnteRichiesta = consumo / voltaggio; %[A]

% dalla scheda tecnica, collector current
correnteMassima2N2222 = 800 / 1000; % [A]

if correnteRichiesta > correnteMassima2N2222
    disp("Non posso alimentare i led solo con il 2N2222")
else
    disp("Posso alimentare i led solo con il 2N2222")
end

%% Calcolo resistenza da alimentatore a collettore transistor

% V = I*R
resistenzaMinimaCollettore = voltaggio / correnteMassima2N2222

%% Calcolo resistenza da pin a base transistor
% l'esp emette 3.3 dai suoi pin
voltPinESP = 3.3; % [V]
% il transistor "mangia" 0.6V (scheda tecnica, base-emitter saturation
% voltage, worst case -> più basso)
voltBaseEmitter = 0.6; % [V]

% la resistenza minima tra esp e transistor per assicurarci i pin non
% esplodano
% V = I*R
resistenzaMinima = (voltPinESP - voltBaseEmitter)/maxCorrenteESP

% proviamo a vedere se questa resistenza va bene
correnteErogata = (voltPinESP - voltBaseEmitter)/470
correnteErogata < maxCorrenteESP

% 470 per sicurezza, tanto non serve avere molta corrente














