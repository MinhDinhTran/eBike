close all
clear all
clc
delete( instrfind)
vh_winfo = instrhwinfo('visa','agilent');
INST_ALL=vh_winfo.ObjectConstructorName;

%CONNECTION SETTINGS BASED ON DEVICE INFO

H_SCOPE=[];
N=size(INST_ALL);
%if isempty(H_SCOPE)
% try to connect to any Rigol scope
for I=1:N(2)
    if ~isempty(strfind(INST_ALL{I}, 'DS')) % seach for the 'sc'ope identificator
        H_SCOPE = eval(INST_ALL{I}); % conect to the first instrument on the USB devices list
        break;
    end;
end;

H_SCOPE.InputBufferSize = 24E6;
H_SCOPE.Timeout = 2;
%H_SCOPE.EOIMode = 'on';

fopen(H_SCOPE);
% identify the scope
fprintf(H_SCOPE, '*IDN?'); ID=fscanf(H_SCOPE);

%fprintf(H_SCOPE, ':RUN'); pause(1); fprintf(H_SCOPE, ':STOP');



% [t, Y1] = getRigolDSOData(H_SCOPE, 'CHAN1',1);
[t, Y2] = getRigolDSOData(H_SCOPE, 'CHAN2',1);
[t, Y3] = getRigolDSOData(H_SCOPE, 'CHAN3',1);
% [t, Y4] = getRigolDSOData(H_SCOPE, 'CHAN4',1);

index = 1;
load('MCU_BLE.mat')

x = min([min(find (Y2 > 1.5)) max(find (Y2 > 1.5))])
x2 = max([max(find (Y3 > 1.5)) min(find (Y3 > 1.5))])
deltaT(index) = t(x2) - t(x)
index = index + 1;

save('MCU_BLE.mat','deltaT','index', 't', 'Y2', 'Y3')

t = t - t(1)
plot(t,Y2 , t,Y3)






















% plot(t,Y1)
% xlabel('t, s'); ylabel('U, V');


% [t, Y2] = getRigolDSOData(H_SCOPE, 'CHAN2',4);

% 
% fprintf(H_SCOPE, ':CHAN1:SCALe?'); UNIT=fscanf(H_SCOPE);
%        plot(Y1);
% xlabel('Samples'); ylabel('RAW ADC');
% drawnow;
%            

% % % % % % % % % % % % % % % % 
% % % % % % % % % % % % % % % DeadTime_PWM1_F = [t, Y1];
% % % % % % % % % % % % % % % save('BEMF','DeadTime_PWM1_F','-append');
% % % % % % % % % % % % % % % DeadTime_PWM1N_F = [t, Y2];
% % % % % % % % % % % % % % % save('BEMF','DeadTime_PWM1N_F','-append');
% PWM3 = [t, Y3];
% save('BEMF','PWM3','-append');
% PWM3N = [t, Y4];
% save('BEMF','PWM3N','-append');
% 
% % % % % % % % % % % % load('BEMF')
% % % % % % % % % % % % figure(1)
% % % % % % % % % % % % 
% % % % % % % % % % % % subplot(2,1,1)
% % % % % % % % % % % % hold on
% % % % % % % % % % % % plot (DeadTime_PWM1_R)
% % % % % % % % % % % % plot (DeadTime_PWM1N_R)
% % % % % % % % % % % % hold off
% % % % % % % % % % % % 
% % % % % % % % % % % % subplot(2,1,2)
% % % % % % % % % % % % hold on
% % % % % % % % % % % % plot (DeadTime_PWM1_F)
% % % % % % % % % % % % plot (DeadTime_PWM1N_F)
% % % % % % % % % % % % hold off
% % % % % % % % % % % % % title(sprintf('BEMF_Retarded'));
% % % % % % % % % % % % 
% % % % % % % % % % % % subplot(6,1,1)
% % % % % % % % % % % % plot (PWM1)
% % % % % % % % % % % % subplot(6,1,2)
% % % % % % % % % % % % plot (PWM1N)
% % % % % % % % % % % % subplot(6,1,3)
% % % % % % % % % % % % plot (PWM2)
% % % % % % % % % % % % subplot(6,1,4)
% % % % % % % % % % % % plot (PWM2N)
% % % % % % % % % % % % subplot(6,1,5)
% % % % % % % % % % % % plot (PWM3)
% % % % % % % % % % % % subplot(6,1,6)
% % % % % % % % % % % % plot (PWM3N)
% % % % % % % % % % % % 
% % % % % % % % % % % % 
% % % % % % % % % % % % xlabel('t, s'); ylabel('U, V');
% % % % % % % % % % % % 
% % % % % % % % % % % % % variklio apviju itampa nuo RPM
% % % % % % % % % % % % plot([30, 50, 70, 90, 110, 130, 150, 180, 200, 230, 250, 270],...
% % % % % % % % % % % %      [0, 3.35, 4.8, 5, 6.3, 7.8, 10.5, 11.3, 14.2, 15.65, 17.5, 20 ])
% % % % % % % % % % % % 


fclose(H_SCOPE);
delete(H_SCOPE);
