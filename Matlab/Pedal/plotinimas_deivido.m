%apsauga suveikimo laikas taisyti!!!!
figure(1)
hold on
yyaxis left
plot(Apsauga_faze(:,2), 'r');
yyaxis right
plotyy(Apsauga_Trigger(:,2), 'blue',Apsauga_MCU(:,2), 'black');
%plotyy(Apsauga_MCU(:,2), 'black');
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt/10)
hold off


%faze ankstinta
figure(2)
plot(BEMF_Advanced(:,1),BEMF_Advanced(:,2));
xlabel('Laikas, ms');
ylabel('�tampa, V')
xlim([-0.002 0.012]);
ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)

%faze normali
figure(3)
plot(BEMF_Normal(:,1),BEMF_Normal(:,2));
xlabel('Laikas, ms');
ylabel('�tampa, V')
xlim([-0.002 0.012]);
ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)

%faze velinta
figure(4)
plot(BEMF_Retarded(:,1),BEMF_Retarded(:,2));
xlabel('Laikas, ms');
ylabel('�tampa, V')
xlim([-0.002 0.012]);
ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)

%3 fazes
figure(5)
hold on
plot(PWM1(:,1),PWM1(:,2))
xlabel('Laikas, ms');
ylabel('�tampa, V')
%xlim([-0.002 0.012]);
%ylim([0 40]);
grid on
xt = get(gca, 'XTick');
set(gca, 'XTick',xt, 'XTickLabel',xt*10000)



