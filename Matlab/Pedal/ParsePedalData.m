function [] = ParsePedalData()
    close all;
    clear all;
    clc;
    load('Data');
    load('D');

    coefsR = [0.002305142887138,0.309399198107079,1.108901860932488e+02]
    coefsL = [0.002977880902536,2.717681357845816,99.586440784918140]

    [pks,locs,w,p] = findpeaks(Data.PedalL.Raw,...
        'MinPeakProminence',80,...
        'MinPeakDistance',40,...
        'Annotate','extents');
    RPM_L = zeros(size(Data.PedalL.T));%diff(locs).*
    RPM_L(locs(2:end)) = datenum(seconds(60))./(datenum(Data.PedalL.T(locs(2:end)) - Data.PedalL.T(locs(1:end-1))));%
    N_L = zeros(size(Data.PedalL.T));
    N_L(locs) = p;
    N_L(locs) = p;

    [pks,locs,w,p] = findpeaks(Data.PedalR.Raw,...
        'MinPeakProminence',40,...
        'MinPeakDistance',20,...
        'Annotate','extents');
    RPM_R = zeros(size(Data.PedalR.T));
    RPM_R(locs(2:end)) = datenum(seconds(60))./(datenum(Data.PedalR.T(locs(2:end)) - Data.PedalR.T(locs(1:end-1))));
    N_R = zeros(size(Data.PedalR.T));
    N_R(locs) = p;
    N_R(locs) = p;
    
%     
%     figure(3) %% test coefs
%     x2 = 1:1:600;
%     y2 = x2.*x2.*coefsR(1)+x2.*coefsR(2);
%     plot(x2,y2)


    N_L = (N_L.^2.*coefsL(1) + N_L.*coefsL(2))./10;
    N_R = (N_R.^2.*coefsR(1) + N_R.*coefsR(2))./10;
%     W_L = N_L.*RPM_L.*(0.17*2*pi/60);
    [RPM_L_1s, ~, ~ ]= ChangeTimeStep(Data.PedalL.T, RPM_L, seconds(1), Data.PedalR.T);
    [RPM_R_1s, ~, ~ ]= ChangeTimeStep(Data.PedalR.T, RPM_R, seconds(1), Data.PedalL.T);
    [N_L_1s, ~, ~ ]= ChangeTimeStep(Data.PedalL.T, N_L, seconds(1), Data.PedalR.T);
    [N_R_1s, ~, ~ ]= ChangeTimeStep(Data.PedalR.T, N_R, seconds(1), Data.PedalL.T);



    W_L_1s = N_L_1s.*RPM_L_1s.*(0.17*2*pi/60);
    W_R_1s = N_R_1s.*RPM_R_1s.*(0.17*2*pi/60);


    s = size(W_L_1s);
    Wh_L = zeros(1,s(2));
    sum = 0;
    for i = 1 : s(2)
        sum = sum + W_L_1s(i) * datenum(seconds(1));
        Wh_L(i) = sum;
    end
    
    s = size(W_R_1s);
    Wh_R = zeros(1,s(2));
    sum = 0;
    for i = 1 : s(2)
        sum = sum + W_R_1s(i) * datenum(seconds(1));
        Wh_R(i) = sum;
    end
    
    I = (D.Current.*0.0217)-44.4+0.6;
    Vbat = (D.Vbat.*0.01400529697297297297297297297297);
    W = I.*Vbat;
    Wh = [];
    s = size(W);
    sum = 0;
    for i = 1 : s(2)
        sum = sum + W(i) * datenum(seconds(1));
        Wh(i) = sum;
    end
    save('PlotPedalData');
end
