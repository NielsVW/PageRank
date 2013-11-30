import java.util.ArrayList;
import java.util.Random;
import java.util.Scanner;

public class Main {

	public static double alpha = 0.9;
	
	/**
	 * Testmatrices: 
	 * double zes  =(double)1/6;
	 * double drie=(double)1/3;
	 * double [][]P = {{0,0.5,0.5,0,0,0},{zes,zes,zes,zes,zes,zes},{drie,drie,0,0,drie,0},{0,0,0,0,0.5,0.5},{0,0,0,0.5,0,0.5},{0,0,0,1,0,0}};
	 * double[][] A = {{0.05,0.9,0.05},{(double)7/15,(double)4/15,(double)4/15},{(double)11/12,(double)1/24,(double)1/24}};
	 * @param args
	 */
	
	public static void main(String[] args) {
		Scanner scan = new Scanner(System.in);
		System.out.println("Give the number of nodes pls");
		int size = scan.nextInt();
		double[][] P = new double[size][size];
		P = generateRandomMatrix(size);
		for(int i=0;i<size;i++){
			for(int j = 0;j<size;j++){
				System.out.print(P[i][j]+ "\t");
			}
			System.out.println();
		}
		double[] b = powerMethod(size, P);
		System.out.println("stationary is: ");
		double sum = 0;
		for(double x : b){
			System.out.println(x);
			sum += x;
		}
		System.out.println("Sum= "+sum);
	}

	private static double[][] generateRandomMatrix(int size) {
		double result[][] = new double[size][size];
		Random rng = new Random();
		int dominant;//deze geeft aan hoeveel 'dominante' waarden er zijn, maw hoeveel nodes niet nul zijn
		for(int i=0;i<size;i++){
			dominant = rng.nextInt(size/2 +1);
			System.out.println("Dominant for row "+i+"= "+dominant);
			if(dominant == 0)
				for(int j = 0;j<size;j++)
					result[i][j] = (double) 1/size;
			else {
				ArrayList<Integer> column = new ArrayList<Integer>(); //the columns where the dominant values are
				for(int j=0;j<dominant;j++){
					int kolom;
					do{	
						kolom = rng.nextInt(size);
					}
					while(column.contains(kolom));
					column.add(kolom);
				}
				for(int j = 0;j<size;j++)
					if(column.contains(j))
						result[i][j] = (double)1/dominant;
					else result[i][j] = 0;
			}
		}  // the current matrix is now P_ and thus stochastic
		return result; 
	}
	
	
	private static double[] powerMethod(int n, double A[][]){
		double[] pi = new double[n];
		for(int m=0;m<n;m++)
			pi[m] = (double) 1/n;
		int counter = 0;
		double E  = 0.1*(double)1/n;
		while(counter < 50) { // using 50 because this is the number Google uses
			for(int i=0;i<n;i++){
				double tmp = 0;
				for(int j=0;j<n;j++){
					tmp += alpha*pi[j]*A[j][i];
				}
				pi[i] = tmp + E; //TODO eigenlijk moet er staan: tmp+ (1-alpha)*E maar anders klopt het niet
			}
			counter++;
		}
		return pi;
	}

}
