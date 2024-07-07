% This file is part of spintach, a tachometer/frequency meter using ATtiny2313A
% Copyright (c) 2024 - Franco Sauvisky
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
%

% Code for increasing PWM resolution using adaptive frequency/period

Nmax = 64;
bsize = 11;
a_range=max(Nmax-2^(bsize-ceil(log2(Nmax))),1):(Nmax-1);
b_range=0:max(a_range);

d =  min(b_range./ a_range',1);

du = unique(d);
du_x = (1:size(du,1))'/size(du,1);

figure(1)
plot(du_x, du,'marker','+','linestyle','none')
hold on
plot(linspace(0,1,size(d,2)),d(end,:),'marker','o','linestyle','none')
grid on
grid minor
hold off

abd = zeros(size(du,1),3);
for du_i = 1:size(du,1)
    [is,js] = find(d==du(du_i));
    [i, ij] = max(is);
    j = js(ij);
    abd(du_i,:) = [a_range(i),b_range(j),du(du_i)];
end

a = abd(:,1);
b = abd(:,2);
d = b./a;

figure(2)
% plot((abd(:,1)'-1)./linspace(0,1,size(abd,1')),'marker','o','color','red')
plot(a,'marker','+','color','red')
hold on
plot(b,'marker','+','color','blue')
% plot(flipud(a-b),'marker','o','color','green','linestyle','none')
% plot(abd(:,end),'marker','o','color','yellow')
axis([0,size(du,1),0,max(a_range)])
grid on
grid minor
hold off

% figure(3)
% plot(abd(1:(end-1),1)-abd(2:end,1),'marker','+','color','red')
% hold on
% plot(abd(1:(end-1),2)-abd(2:end,2),'marker','+','color','blue')
% % plot(,'marker','+','color','black')
% % plot(floor((abd(1:(end-1),1)-abd(2:end,1))./((abd(1:(end-1),1))./(abd(1:(end-1),2)))),'marker','+','color','black')
% % plot((abd(1:(end-1),2)-abd(2:end,2))./(abd(1:(end-1),1)-abd(2:end,1)),'marker','+','color','green')
% grid on
% grid minor
% hold off

printf("LUP Size: %d bytes\n",ceil(bsize/8*size(du,1)/2));
printf("Points: %d\n",size(du,1));
printf("P/B: %d\n",size(du,1)/ceil(bsize/8*size(du,1)/2));