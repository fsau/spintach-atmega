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