%apsauga suveikimo laikas taisyti!!!!
figure(1)
    set(findall(gca, 'Type', 'Line'),'LineWidth',10);

yyaxis left
plot(Apsauga_faze(:,1), Apsauga_faze(:,2), 'b');
yyaxis right
hold on
plot(Apsauga_Trigger(:,1), Apsauga_Trigger(:,2), 'r');
plot(Apsauga_MCU(:,1), Apsauga_MCU(:,2), '-black');
hold off

xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt/10)


%faze ankstinta
figure(2)
subplot (1,3,1)
plot(BEMF_Advanced(:,1)+0.002,BEMF_Advanced(:,2));
xlabel('Laikas, ms');
ylabel('Átampa, V')
xlim([0 0.015]);
ylim([0 40]);
grid on

subplot (1,3,2)
plot(BEMF_Normal(:,1)+0.002,BEMF_Normal(:,2));
xlabel('Laikas, ms');
ylabel('Átampa, V')
xlim([0 0.015]);
ylim([0 40]);
grid on

subplot (1,3,3)
plot(BEMF_Retarded(:,1)+0.002,BEMF_Retarded(:,2));
xlabel('Laikas, ms');
ylabel('Átampa, V')
xlim([0 0.015]);
ylim([0 40]);
grid on


%3 kanalu pwm
figure(5)
hold on
subplot(6,1,1)
plot (PWM1(:,1)+0.006, PWM1(:,2))
ylim([0.01 3.5]);
%xlim([-0.002 0.012]);
subplot(6,1,2)
plot (PWM1N(:,1)+0.006, PWM1N(:,2))
ylim([0.01 3.5]);
subplot(6,1,3)
plot (PWM2(:,1)+0.006, PWM2(:,2))
ylim([0.01 3.5]);
subplot(6,1,4)
plot (PWM2N(:,1)+0.006, PWM2N(:,2))
ylim([0.01 3.5]);
subplot(6,1,5)
plot (PWM3(:,1)+0.006, PWM3(:,2))
ylim([0.01 3.5]);
subplot(6,1,6)
plot (PWM3N(:,1)+0.006, PWM3N(:,2))
ylim([0.01 3.5]);
xlabel('Laikas, ms');
ylabel('Átampa, V')
%xlim([-0.002 0.012]);
%ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt/3.33)


lon =PWM1(:,2);
lonWrapped = wrapTo360(PWM1(:,2));
plot(lonWrapped)


%deadtime
figure(6)

subplot(1,2,1)
hold on
plot(DeadTime_PWM1_F(:,1)+2*10^-7, DeadTime_PWM1_F(:,2));
plot(DeadTime_PWM1N_F(:,1)+2*10^-7, DeadTime_PWM1N_F(:,2));
hold off
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 6*10^-7]);
ylim([-0.5 4]);
grid on

subplot(1,2,2)
hold on
plot(DeadTime_PWM1_R(:,1)+2*10^-7, DeadTime_PWM1_R(:,2));
plot(DeadTime_PWM1N_R(:,1)+2*10^-7, DeadTime_PWM1N_R(:,2));
legend('Teigiamas signalas','Neigiamas signalas')
hold off
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 6*10^-7]);
ylim([-0.5 4]);
grid on

%3 fazes
figure(7)

subplot(3,1,1)
hold on
plot(Phase1(:,1)+0.012,Phase1(:,2));
hold off
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 0.024]);
ylim([0 40]);
grid on

subplot(3,1,2)
hold on
plot(Phase2(:,1)+0.012,Phase2(:,2));
hold off
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 0.024]);
ylim([0 40]);
grid on

subplot(3,1,3)
hold on
plot(Phase3(:,1)+0.012,Phase3(:,2));
hold off
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 0.024]);
ylim([0 40]);
grid on

%ADC itampos matavimo velinimas

figure(8)

subplot(1,2,1)
hold on
plot(ADC_Delay(:,1)+0.000045,ADC_Delay(:,2)/10);
plot(ADC_Delay_LED(:,1)+0.000045,ADC_Delay_LED(:,2));
hold off
legend('Gráþtamos EVJ impulsas','ADC matavimo signalas')
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 0.00003]);
ylim([0.01 4]);
grid on

subplot(1,2,2)
hold on
plot(ADC_Delay2(:,1)+0.000045,ADC_Delay2(:,2)/10);
plot(ADC_Delay2_LED(:,1)+0.000045,ADC_Delay2_LED(:,2));
hold off
legend('Gráþtamos EVJ impulsas','ADC matavimo signalas')
xlabel('Laikas, s');
ylabel('Átampa, V')
xlim([0 0.00003]);
ylim([0.01 4]);
grid on

%integravimo ir integruojamu duomenu pav

figure(9)

subplot(3,1,1)
hold on 
plot(Fronts(:,1)+0.03,Fronts(:,2));
hold off
xlabel('Laikas, s');
ylabel('Gryþtama EVJ')
grid on
ylim([-0.01 2]);

subplot(3,1,2)
hold on 
plot(Integral_LED(:,1)+0.03,Integral_LED(:,2));
hold off
xlabel('Laikas, s');
ylabel('frontai')
grid on
ylim([-0.01 3.3]);

subplot(3,1,3)
hold on 
plot(Integral(:,1)+0.03,Integral(:,2));
hold off
xlabel('Laikas, s');
ylabel('Integralas')
grid on
ylim([-0.01 3.3]);

