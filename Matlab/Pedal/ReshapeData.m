close all;
clear all;
clc;
load('Data');

minT = min([min(Data.Vbat.T), min(Data.Current.T),...
            min(Data.Speed.T), min(Data.Power.T),...
            min(Data.PedalL.T), min(Data.PedalR.T),...
            min(Data.Mode.T), min(Data.Duty.T)]);
maxT = max([max(Data.Vbat.T), max(Data.Current.T),...
            max(Data.Speed.T), max(Data.Power.T),...
            max(Data.PedalL.T), max(Data.PedalR.T),...
            max(Data.Mode.T), max(Data.Duty.T)]);

%         
% minT = min([min(Data.PedalL.T), min(Data.Duty.T)]);
% maxT = max([max(Data.PedalL.T), max(Data.Duty.T)]);
TimeBase = minT:seconds(10):maxT;

D.T = TimeBase;
fields = fieldnames(Data);
sizex = size(fields);
sizey = size(TimeBase);
for ii = 1:size(fields)
    arrayRAW = getfield(getfield(Data, fields{ii}), 'Raw');
    arrayT = getfield(getfield(Data, fields{ii}), 'T');
    tim_index = 1;
    
    sum = 0;
    count = 0;
    rez = zeros(size(TimeBase));
    minn = zeros(size(TimeBase));
    maxx = zeros(size(TimeBase));
    temp = 0;
    lastValue = 0;
    for i = 1:size(arrayT)-1
        if(temp == 0)
            while (tim_index <= sizey(2)  &&  arrayT(i) > TimeBase(tim_index))
                rez(tim_index) = lastValue;
                tim_index = tim_index +1;
            end
            temp = 1;
        end
        if (tim_index <= sizey(2)  &&  arrayT(i) <= TimeBase(tim_index))
            sum = sum + arrayRAW(i);
            if (minn(tim_index) > arrayRAW(i) || minn(tim_index) == 0)
                minn(tim_index) = arrayRAW(i);
            end
            if (maxx(tim_index) < arrayRAW(i))
                maxx(tim_index) = arrayRAW(i);
            end
            if (arrayRAW(i) > 0)
                count = count + 1;
            end
        else
            if (tim_index <= sizey(2))
                temp = 0;
                rez(tim_index) = sum/count;
                if (isnan(rez(tim_index)))
                    rez(tim_index) = lastValue;
                end;
                lastValue = rez(tim_index);
                tim_index = tim_index +1;
                sum = 0;
                count = 0;
            end
        end
    end
    D.(fields{ii}) = rez;
    Minn.(fields{ii}) = minn;
    Maxx.(fields{ii}) = maxx;
end

Minn.T = TimeBase;
Maxx.T = TimeBase;
clearvars -except D Minn Maxx
save('D');








% TimeBase = 0:seconds(1):minutes(3);
% 
%    
% figure(1)
% yyaxis left
% plot (Data.Current.T, Data.Current.D, 'b')
% yyaxis right
% hold on
% plot (Data.Vbat.T, Data.Vbat.Raw*Data.Current.D, 'r')
% % plot (Data.Duty.T, Data.Duty.Raw, 'r')
% % plot (Data.PedalL.T, Data.PedalL.Raw, 'r')
% % plot (Data.PedalR.T, Data.PedalR.Raw, 'g')
% hold off





% hold on
% plot (C{5}{6}, str2double(C{5}{3}), 'red')%L pedalas
% plot (C{6}{6}, str2double(C{6}{3}), 'green')%R pedalas
% hold off

% for fileNo = 1:size(listing)-2
%     for i = 1:size(C{fileNo}{2})
%     C{fileNo}{2}{i}(9) = '.';
%     C{fileNo}{2}{i}(13) = ' ';
%     end
% end

%     public static final String UUID_MODE_ID = "0000fff1-0000-1000-8000-00805f9b34fb";
%     public static final String UUID_PWM_DUTY_CYCLE_ID = "0000fff2-0000-1000-8000-00805f9b34fb";
%     public static final String UUID_V_THRESHOLD_ID = "0000fff3-0000-1000-8000-00805f9b34fb";


%     public static final String UUID_BIKE_BATTERY_LEVEL_ID = "0000fff4-0000-1000-8000-00805f9b34fb";
%     public static final String UUID_CURRENT_ID = "0000fff5-0000-1000-8000-00805f9b34fb";
%     public static final String UUID_BIKE_SPEED_ID = "0000fff6-0000-1000-8000-00805f9b34fb";
%     public static final String UUID_BIKE_FLAGS_ID = "0000fff7-0000-1000-8000-00805f9b34fb";
% 
% figure(1)
% yyaxis left
% plot (C{3}{6}, -44.4+str2double(C{3}{3})*0.0217, 'b')%srove
% yyaxis right
% hold on
% plot (C{5}{6}, str2double(C{5}{3}), 'red')%L pedalas
% plot (C{6}{6}, str2double(C{6}{3}), 'green')%R pedalas
% hold off
% 
% plot (C{8}{6}, str2double(C{8}{3}),'r')%duty
% 
% hold on
% plot (C{1}{6}, str2double(C{1}{3}), 'red')%pulsas
% plot (C{2}{6}, str2double(C{2}{3}), 'red')%baterijos V
% plot (C{3}{6}, str2double(C{3}{3}), 'red')%srove
% plot (C{4}{6}, str2double(C{4}{3}), 'red')%greitis?
% plot (C{5}{6}, str2double(C{5}{3}), 'red')%L pedalas
% plot (C{6}{6}, str2double(C{6}{3}), 'red')%R pedalas
% plot (C{7}{6}, str2double(C{7}{3}), 'red')%mode
% plot (C{8}{6}, str2double(C{8}{3}), 'red')%duty
% plot (C{9}{6}, str2double(C{9}{3}), 'red')%v thr
% hold off
% 
% 
% 
% Fs = 5000;            % Sampling frequency
% T = 1/Fs;             % Sampling period
% L = 100000;             % Length of signal
% t = (0:L-1)*T;        % Time vector
% % plot(1000*t(1:50),X(1:50))
% Y = fft(str2double(C{5}{3}));
% 
% P2 = abs(Y/L);
% P1 = P2(1:L/2+1);
% P1(2:end-1) = 2*P1(2:end-1);
% 
% f = Fs*(0:(L/2))/L;
% plot(f,P1)
% 
% 
% a = 1;
% b = [1 1];
% y = filter(b,a,double(C{3}{5}));
% plot(y)
% 


% figure(2)
% plot(C{2}{5}-C{3}{5})
%
% figure(2)
% hold on
% plot (C{2}{5})
% plot (C{3}{5})
% hold off