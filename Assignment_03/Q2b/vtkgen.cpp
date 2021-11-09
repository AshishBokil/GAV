#include <bits/stdc++.h>
using namespace std;

#define inf INT_MAX
#define ll long long int
#define ld long double

int xshift[9] = {-1, 0, 1, 1, 1, 0, -1, -1, 0};
int yshift[9] = {-1, -1, -1, 0, 1, 1, 1, 0, 0};

bool ismax(ld *grid, int i, int j, int x)
{
    if (grid[i + x * j] < -5)
        return false;
    for (int k = 0; k < 8; k++)
    {
       // cout<<i+xshift[k]+x*(j+yshift[k])<<endl;
        ll index = (i + xshift[k] + x * (j + yshift[k]));
       // cout<<index<<endl;
        if (grid[index] < -5 || grid[index] > grid[i + x * j])
        {
            //  cout<<setprecision(12)<<grid[index] <<" \t" <<grid[i+x*j]<<endl;
            return false;
        }
    }

    return true;
}

int main()
{
    ifstream input;
    //	unsigned int x, y, z;
    input.open("ass3_ocean.vtk");
    int x, y, z;
    input >> x;
    input >> y;
    input >> z;

    ll noOfpoints = x * y * z;

    ll count = 1;
    ld *grid = new ld[noOfpoints];

    for (int k = 0; k < z; k++)
    {
        for (int j = 0; j < y; j++)
        {
            for (int i = 0; i < x; i++)
            {

                input >> grid[i + x * j];
                //  cout<<setprecision(12)<<grid[i+x*j]<<" ";
            }
            // cout<<"\n\n";
        }
    }
    input.close();

    ofstream output;
    output.open("ass3_ocean_out.vtk");
    output << setprecision(12);
    output << x << " ";
    output << y << " ";
    output << z << " \n";
    for (int j = 0; j < y; j++)
    {
        for (int i = 0; i < x; i++)
        {

            if (i == 0 || j == 0 || i == x - 1 || j == y - 1)
            {
                output << grid[i + x * j] << " ";
                continue;
            }
            bool result = ismax(grid, i, j, x);
            if (result)
            {
                cout << count << " -> " << i << "," << j << "   = " << result << endl;
                count++;
               // grid[i + x * j] = 6;
                output <<"6.0000000 ";
            }
            else
            {
                output << grid[i + x * j] << " ";
            }
        }

        //  output << "\n\n";
    }

    //fclose(output);
    output.close();
    return 0;
}