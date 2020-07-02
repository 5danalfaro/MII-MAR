%RESULTADOS%

%Dibujar entrada de clientes en parada par_plot%
figure;
par_plot=23;
nom= paradas{1}{par_plot}('NOMBRE');
plot((1:240)/12,paradas{3}{par_plot}((23*24):(23*24+239)))
title(strcat('Entrada de clientes primeros 20 minutos en parada: ',nom));
ylabel('Booleana de entrada');
xlabel('Tiempo en (minutos)');

%Tiempos de recorrido de los dos primeros autobuses
figure;
a=1:36;
bar(a-1.2,autobuses{3}{1}(1:length(autobuses{3}{1})-1)/12,0.2,'DisplayName','autobus 1')
hold('on');
bar(a-1,autobuses{3}{1}(1:length(autobuses{3}{1})-1)/12,0.2,'DisplayName','autobus 2')
xticks(a);
title('Tiempos de recorrido de los dos primeros autobuses','FontSize', 18);
ylabel('Tiempos de recorrido (min)','FontSize', 12);
xlabel('Recorrido','FontSize', 12);
legend


%Dibujar histograma de tiempos de espera de clientes%
figure
c_sist=0;
for n = 1:n_total_clientes
    if(clientes{1}{n}('TI')>0)
        c_sist=c_sist+1;
        time_passed=1/12*(clientes{1}{n}('TF')-clientes{1}{n}('TI'));
        u(c_sist)=time_passed;
    end
    
end
a=hist(u);
hist(u);
hold('on');
plot(mean(u)*ones(1,20),1:max(a)/20:max(a));
p_tit=num2str(round(mean(u),2));
text(mean(u),max(a)*16/20,strcat('media de espera:',p_tit,' min.'),'FontSize', 12);
p_title='Histograma de tiempo de espera';
title(p_title,'FontSize', 18);
xlabel('Tiempo (min)','FontSize', 12);
ylabel('Clientes','FontSize', 12);

%Dibujar histograma de tiempos de recorrido de buses%


figure
u=[];
a_i=(min(auto_en_linea)-1);
for n = 1:a_i
    time_passed=1/12*(autobuses{1}{n}('TF')-autobuses{1}{n}('TI'));
    u(n)=time_passed;
end

tiledlayout(2,1)

% plot de la izquierda
ax1 = nexttile;

a=hist(u);
hist(ax1,u);
hold(ax1,'on');
plot(ax1,mean(u)*ones(1,20),1:max(a)/20:max(a));
p_tit=num2str(round(mean(u),2));
text(ax1,mean(u),max(a)*16/20,strcat('media de recorrido de autobuses: ',p_tit,' min.'),'FontSize', 12,'Color', 'r');
p_title='Histograma de tiempo de circulacion de toda la linea';
title(ax1,p_title,'FontSize', 14);
xlabel(ax1,'Tiempo (min)','FontSize', 12);
ylabel(ax1,'Autobuses','FontSize', 12);

% plot de la derecha
ax2 = nexttile;

bar(ax2,1:a_i,u);
p_title='Gráfico de tiempo de circulación de cada autobus';
title(ax2,p_title,'FontSize', 14);
xlabel(ax2,'Tiempo total(min)','FontSize', 12);
ylabel(ax2,'ID de los autobuses','FontSize', 12);
