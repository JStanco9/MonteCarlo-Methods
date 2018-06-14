//created by John Stanco 6.2.18

#include "../include/PIMC_State.hpp"
#include "../include/Metropolis_Hastings_MC.hpp"
#include <chrono>

double
gaussian(double x){
	double a = pow(2 * pi, -.5);
	return a * exp(-x * x / 2);
}

double printNormal(uint n){
	std::string filename = "../data/normDist.dat";
	FILE *pFile = fopen(filename.c_str(), "w");
	for(uint i = 0; i < n; i++){
		fprintf(pFile, "%d\t%f\n", 0, randNorm());
	}
	fclose(pFile);
	return 1;
}

double
mean(arma::vec dist){
	double u = 0;
	for(uint i = 0; i < dist.size(); i++){
		u += dist(i);
	}
	return u / dist.size();
}


double
variance(arma::vec dist){
	double v = 0;
	double u = mean(dist);
	for(uint i = 0; i < dist.size(); i++){
		v += pow(dist(i), 2);
	}
	v /= dist.size();
	return v - pow(u, 2);
}


double
mean(arma::mat pmf){
	double u = 0;
	double tot = 0;
	for(uint i = 0; i < pmf.n_rows; i++){
		u += pmf(i, 0) * pmf(i, 1);
		tot += pmf(i, 1);
	}
	return u / tot;
}


double
variance(arma::mat pmf){
	double u = mean(pmf);
	double v = 0;
	double tot = 0;
	for(uint i = 0; i < pmf.n_rows; i++){
		v += pow(pmf(i, 0), 2) * pmf(i, 1);
		tot += pmf(i, 1);
	}
	return v / tot - pow(u, 2);
}


arma::mat
pmf(arma::vec dist, double xmin, double xmax){
	double n = 100;
	arma::vec sorted = sort(dist);
	arma::mat masses(n, 2);
	double x = xmin;
	uint i = 0;
	double dx = (xmax - xmin) / (n - 1);
	for(uint j = 0; j < n; j++){
		x += dx;
		masses(j, 0) = x - dx / 2;
		//count number of elements in range x -> x + dx
		while(i < dist.size() && sorted(i) < x ){
			masses(j, 1) += 1;
			i++;
		}
	}
	masses.col(1) *= pow(dist.size(), -1) * n / 10;
	return masses;
}


arma::mat
gaussianPmf(double xmin, double xmax){
	double n = 1000;
	arma::mat gauss(n, 2);
	double x = xmin;
	double dx = (xmax - xmin) / (n - 1);
	for(uint j = 0; j < n; j++){
		x += dx;
		gauss(j, 0) = x - dx / 2;
		gauss(j, 1) = gaussian(gauss(j, 0));
	}
	return gauss;
}


int
printfPmf(arma::mat pmf, std::string filename){
	FILE *pFile = fopen(filename.c_str(), "w");
	for(uint i = 0; i < pmf.n_rows; i++){
		fprintf(pFile, "%f\t%f\n", pmf(i, 0), pmf(i, 1));
	}
	fclose(pFile);
	return 1;
}


int testRNG(){
	uint samples = 1e7;
	printf("-------------------------------------------------------------\n");
	clock_t t = clock();
	arma::mat normal = normDistVec(samples);
	t = clock() - t;
	printf("Time to compute %d normal samples:\t%f sec\n", samples, (double)t / CLOCKS_PER_SEC);
	printf("-------------------------------------------------------------\n");
	t = clock();
	arma::mat uniform = uniDistVec(samples);
	t = clock() - t;
	printf("Time to compute %d uniform samples:\t%f sec\n", samples, (double)t / CLOCKS_PER_SEC);

	printf("-------------------------------------------------------------\n");

	arma::mat normalPmf = pmf(normal, -5, 5);
	arma::mat uniPmf = pmf(uniform, -5, 5);

	printf("mean of sampled normal:\t\t%f\n", mean(normalPmf));
	printf("variance of sampled normal:\t%f\n", variance(normalPmf));
	printf("-------------------------------------------------------------\n");
	printf("mean of sampled uniform:\t%f\n", mean(uniPmf));
	printf("variance of sampled uniform:\t%f\n", variance(uniPmf));
	printf("-------------------------------------------------------------\n");
	std::string filename = "../data/normal.dat";
	printfPmf(normalPmf, filename);
	filename = "../data/uniform.dat";
	printfPmf(uniPmf, filename);
	return 1;
}


//need to: set the positions of particles via...
//given a lattice, we should have some way to generate the cube pos.
//This works by periodically repeating the lattice...(can use bravais lattice selector from tb)
//This would shift us to a general condensed matter interface .. See the transition to a type of COMSOL
//but for condensed matter.. (Is this as reliable as COMSOL?)


//TODO

//	1) Check if masses are being copied
//	2) Produce pos from masses
//	4) Must store every position vector(unavoidable)
//	5) Masses are never copied, so not as difficult
//	6) Fix printing (Be able to print every state in chain)
//		Maybe have some internal method (get data) that prints to a file
//		Define an << operator that prints to some stream...			
//	7) Also consider the strength of potential between
//		different atoms
//		this must also be stored in some sort of dictionary
//		Define this data structure:
//		- Lattice -> all atom-atom interaction potential info
//					 mass of each atom
//					 Index each atom in the lattice {0 ... N - 1}
//					 Index of atom in simulation is i -> atom type is i % N;
//					 n^2 atom-interactions? 
//					 Store some periodic table inside sim...
//					 Lennard Jones potential info...tables available online


//We want our states to be able to...
//



int main(){
	srand(time(NULL));
	double T = 1;
	uint n_slice = 64;
	uint n_part = 4;


	arma::vec masses(arma::ones(n_part) * 2);
	arma::vec sigma(arma::ones(n_part) * 2);
	arma::vec eps(arma::ones(n_part) * 10);
	arma::mat pos(2, n_part);


	arma::vec r1;
	arma::vec r2;
	arma::vec r3;
	arma::vec r4;


	r1 << 1 << 1 << arma::endr;
	r2 << 1 << -1 << arma::endr;
	r3 << -1 << 1 << arma::endr;
	r4 << -1 << -1 << arma::endr;


	pos.col(0) = r1;
	pos.col(1) = r2;
	pos.col(2) = r3;
	pos.col(3) = r4;


	Metropolis_Hastings_MC<PIMC_State> PIMC;

	uint iter = 80000;

	PIMC_State init = PIMC_State::builder().setTemp(T)
											.setSlices(n_slice)
											.setMasses(masses)
											.setSigma(sigma)
											.setEps(eps)
											.setPos(pos)
											.build();


	for(unsigned i = 0; i < 1; i++){
		printf("-------------------------------------------------------------\n");

		clock_t t = clock();
		for(uint j = 0; j < iter; j++){
			init.update();
		}
		//PIMC.run(&init, iter);
		t = clock() - t;
		printf("Time to run %d PIMC iterations:\t%f sec\n", iter, (double)t / CLOCKS_PER_SEC);

		printf("-------------------------------------------------------------\n");
		init.printToFile("../data/PIMC.dat");
		//PIMC.getChain()[iter - 1]->printToFile("../data/PIMC.dat");
	}
	return 1;
	//return testRNG();
}