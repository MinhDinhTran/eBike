%apsauga suveikimo laikas taisyti!!!!
figure(1)

yyaxis left
plot(Apsauga_faze(:,2), 'r');
yyaxis right
hold on
plot(Apsauga_Trigger(:,2), 'blue');
plot(Apsauga_MCU(:,2), '-black');
hold off

xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt/10)


%faze ankstinta
figure(2)
plot(BEMF_Advanced(:,1),BEMF_Advanced(:,2));
xlabel('Laikas, ms');
ylabel('Átampa, V')
xlim([-0.002 0.012]);
ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)

%faze normali
figure(3)
plot(BEMF_Normal(:,1),BEMF_Normal(:,2));
xlabel('Laikas, ms');
ylabel('Átampa, V')
xlim([-0.002 0.012]);
ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)

%faze velinta
figure(4)
plot(BEMF_Retarded(:,1),BEMF_Retarded(:,2));
xlabel('Laikas, ms');
ylabel('Átampa, V')
xlim([-0.002 0.012]);
ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)

%3 kanalu pwm
figure(5)
hold on
subplot(6,1,1)
plot (0:360/1200:360, PWM1(:,2))
subplot(6,1,2)
plot (PWM1N(:,2))
subplot(6,1,3)
plot (PWM2(:,2))
subplot(6,1,4)
plot (PWM2N(:,2))
subplot(6,1,5)
plot (PWM3(:,2))
subplot(6,1,6)
plot (PWM3N(:,2))
xlabel('Laikas, ms');
ylabel('Átampa, V')
%xlim([-0.002 0.012]);
%ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt/3.33)


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
plot(Phase1(:,1),Phase1(:,2));
hold off
xlabel('Laikas, ms');
ylabel('Átampa, V')
%xlim([-0.002 0.012]);
ylim([0 40]);
grid on

subplot(3,1,2)
hold on
plot(Phase2(:,1),Phase2(:,2));
hold off
xlabel('Laikas, ms');
ylabel('Átampa, V')
%xlim([-0.002 0.012]);
%ylim([0 40]);
grid on

subplot(3,1,3)
hold on
plot(Phase3(:,1),Phase3(:,2));
hold off
xlabel('Laikas, ms');
ylabel('Átampa, V')
%xlim([-0.002 0.012]);
%ylim([0 40]);
grid on
