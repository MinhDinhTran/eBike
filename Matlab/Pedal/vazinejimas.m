load('pasivazinejimas_visi')

T = cat(2, DD1.T, DD2.T, DD.T)
HR = cat(2, DD1.HR, DD2.HR, DD.HR)
Vbat = cat(2, DD1.Vbat, DD2.Vbat, DD.Vbat)
Current = cat(2, DD1.Current, DD2.Current, DD.Current)
Speed = cat(2, DD1.Speed, DD2.Speed, DD.Speed)
Power = cat(2, DD1.Power, DD2.Power, DD.Power)
PedalL = cat(2, DD1.PedalL, DD2.PedalL, DD.PedalL)
PedalR = cat(2, DD1.PedalR, DD2.PedalR, DD.PedalR)
Mode = cat(2, DD1.Mode, DD2.Mode, DD.Mode)
Duty = cat(2, DD1.Duty, DD2.Duty, DD.Duty)

x= find(Vbat<2000)
Vbat(x) = NaN;
Vbat = fillmissing(Vbat,'previous');

x= find(Current<500)
Current(x) = NaN;
Current = fillmissing(Current,'previous');

Current = (Current.*0.0217)-44.4+0.6;
Vbat = (Vbat.*0.01400529697297297297297297297297);
x= find(isnan(Current))
Current(x) = 0;
x= find(isnan(Vbat))
Vbat(x) = 0;






x = find(Current<0);

figure(1)
plot(T(x), Vbat(x), T(x), Current(x))


x = find(Current>0);

figure(2)

plot(T(x), Vbat(x), T(x), Current(x))



W = Current.*Vbat;
W = movmean(W, 2);

figure(5)

plot(T, W)






xr = find(W>0);
Wr =W(xr);

xv = find(W<0);
Wv =W(xv);

Wh = [];
s = size(W);
for i = 1 : s(2)
    Wh(i) = mean(W(1:i)) * ((T(i)-T(1))/hours(1));
end
Whr = [];
s = size(Wr);
for i = 1 : s(2)
    Whr(i) = mean(Wr(1:i)) * ((T(i)-T(1))/hours(1));
end
Whv = [];
s = size(Wv);
for i = 1 : s(2)
    Whv(i) = mean(Wv(1:i)) * ((T(i)-T(1))/hours(1));
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(3)
subplot(6,1,1)
plot(T, W)

subplot(6,1,2)
plot(T, Wh)

subplot(6,1,3)
x = find(W>0);
plot(T(x), W(x))

subplot(6,1,4)
plot(T(xr), Whr)



s = size(T);
whrrr = zeros(1,s(2));
whrrr(xv) = W(xv);
x = find(whrrr==0);
whrrr(x) = 0;

subplot(6,1,5)
plot(T, -whrrr)



% subplot(6,1,5)
% x = find(W<0);
% plot(T(x), -W(x))

subplot(6,1,6)
plot(T(xv), -Whv)


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
x = find(W>0);

figure(4)
plot(T(x), W(x))


x = find(W<0);

figure(6)
plot(T(x), W(x))

