#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;


int main()
{
    PolygonalMesh mesh;


    if(!ImportMesh(mesh))
    {
        cerr << "File not found" << endl;
        return 1;
    }
    else { 
        //all markers are correctly stored
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.MarkerCell0Ds.begin(); it != mesh.MarkerCell0Ds.end(); it++)
        {
            cout << "Marker: " << it -> first <<", Values:";

            for(const unsigned int id : it -> second)
            {
                cout << " " << id;
            }

        cout << endl;
        }

        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.MarkerCell1Ds.begin(); it != mesh.MarkerCell1Ds.end(); it++)
        {
            cout << "Marker: " << it -> first <<", Values:";
            
            for(const unsigned int id : it -> second)
            {
                cout << " " << id;
            }

        cout << endl;
        }

        cout << "Cell2D marker:" << endl;
        for(auto it = mesh.MarkerCell2Ds.begin(); it != mesh.MarkerCell2Ds.end(); it++)
        {
            cout << "Marker: " << it -> first <<", Values:";
            
            for(const unsigned int id : it -> second)
            {
                cout << " " << id;
            }

        cout << endl;
        }

    //each edge had non-zero lenght --> per Cell2Ds (lati poligono)
        cout << mesh.NumCell2Ds << endl;
    for(unsigned int poligono = 0; poligono < mesh.NumCell2Ds; poligono++)
    {
    vector<unsigned int> edges = mesh.Cell2DsEdges[poligono];

    for(unsigned int e = 0; e < edges.size(); e++)
    {
        const unsigned int origin = mesh.Cell1DsExtrema(0, edges[e]);
        
        const unsigned int end = mesh.Cell1DsExtrema(1, edges[e]);

        auto findOrigin = find(mesh.Cell2DsVertices[poligono].begin(), mesh.Cell2DsVertices[poligono].end(), origin);
        if(findOrigin == mesh.Cell2DsVertices[poligono].end())
        {
        cerr << "Wrong mesh" << endl;
        return 2;
        }

        auto findEnd = find(mesh.Cell2DsVertices[poligono].begin(), mesh.Cell2DsVertices[poligono].end(), end);
        if(findEnd == mesh.Cell2DsVertices[poligono].end())
            {
            cerr << "Wrong mesh" << endl;
            return 3;
            }
        
        double x_origin = mesh.Cell0DsCoordinates(0, origin);
        double y_origin = mesh.Cell0DsCoordinates(1, origin);
        double x_end = mesh.Cell0DsCoordinates(0, end);
        double y_end = mesh.Cell0DsCoordinates(1, end);

        if (abs(x_origin - x_end) < 1e-9 && abs(y_origin - y_end) < 1e-9){
            cout << "Il segmento ha lunghezza 0";
            return 4;
        }
    }
    
        //each polygon has a non-zero area --> per Cell2Ds (area poligono)
        double area = 0;
        double x_origin, y_origin, x_end, y_end;
        int origin = mesh.Cell1DsExtrema(0, edges[0]);
        int end = mesh.Cell1DsExtrema(1, edges[0]);
        bool first = true;
        bool poliValid = true;

        while( (origin != mesh.Cell1DsExtrema(0, edges[0])  || first ) && poliValid ){
            if (first){
                first = false;
            }

            x_origin = mesh.Cell0DsCoordinates(0, origin); 
            y_origin = mesh.Cell0DsCoordinates(1, origin);
            x_end = mesh.Cell0DsCoordinates(0, end);
            y_end = mesh.Cell0DsCoordinates(1, end);

            area += 0.5 * (x_origin*y_end - x_end*y_origin);

            poliValid = false;
            for(unsigned int i = 0; i < edges.size() && !poliValid; i++){
                if(mesh.Cell1DsExtrema(0, edges[i]) == end ){
                    origin = mesh.Cell1DsExtrema(0, edges[i]); 
                    end = mesh.Cell1DsExtrema(1, edges[i]);
                    poliValid = true;
                }
            }
        }
        if(poliValid)
            cout << "Area poligono " << poligono << ": " << area << endl;
        else
            cout << "Poligono " << poligono << " non valido" << endl;


    }


    //the loaded mesh is identical to the one shown in the provided images

        //Esporto Cell0Ds
        Gedim::UCDUtilities utilities;
        {
            vector<Gedim::UCDProperty<double>> cell0Ds_properties(1);

            cell0Ds_properties[0].Label = "Marker";
            cell0Ds_properties[0].UnitLabel = "-";
            cell0Ds_properties[0].NumComponents = 1;

            vector<double> cell0Ds_marker(mesh.NumCell0Ds, 0.0);
            for(const auto &m : mesh.MarkerCell0Ds)
                for(const unsigned int id: m.second)
                    cell0Ds_marker.at(id) = m.first;

            cell0Ds_properties[0].Data = cell0Ds_marker.data();

            utilities.ExportPoints("./Cell0Ds.inp",
                                mesh.Cell0DsCoordinates,
                                cell0Ds_properties);
        }

    // Esporto Cell1Ds
        {

            vector<Gedim::UCDProperty<double>> cell1Ds_properties(1);

            cell1Ds_properties[0].Label = "Marker";
            cell1Ds_properties[0].UnitLabel = "-";
            cell1Ds_properties[0].NumComponents = 1;

            vector<double> cell1Ds_marker(mesh.NumCell1Ds, 0.0);
            for(const auto &m : mesh.MarkerCell1Ds)
                for(const unsigned int id: m.second)
                    cell1Ds_marker.at(id) = m.first;

            cell1Ds_properties[0].Data = cell1Ds_marker.data();

            utilities.ExportSegments("./Cell1Ds.inp",
                                    mesh.Cell0DsCoordinates,
                                    mesh.Cell1DsExtrema,
                                    {},
                                    cell1Ds_properties);
        }




    }
        return 0;
    }

