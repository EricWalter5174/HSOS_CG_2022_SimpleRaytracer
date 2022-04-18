//
//  SimpleRayTracer.cpp
//  SimpleRayTracer
//
//  Created by Philipp Lensing on 03.09.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "SimpleRayTracer.h"
#include "rgbimage.h"
#include <stdio.h>
#include <math.h>


Camera::Camera(float zvalue, float planedist, float width, float height, unsigned int widthInPixel, unsigned int heightInPixel)
{
	zwert = zvalue;
	distanzZuEbene = planedist;
	breite = width;
	hoehe = height;
	breiteInPixel = widthInPixel;
	hoeheInPixel = heightInPixel;
}

Vector Camera::generateRay(unsigned int x, unsigned int y) const
{
	//Berechne Mitte von Höhe und Breite
	float breiteMitte = (float)this->breiteInPixel / 2;
	float hoeheMitte = (float)this->hoeheInPixel / 2;
	//Berechne die Breite und Höhe der Pixel
	float breiteProPixel = this->breite / this->breiteInPixel;
	float hoeheProPixel = this->hoehe / this->hoeheInPixel;

	//Bestimme delta x und delta y für Bestimmung des Strahls von Augpunkt zu Pixel
	int deltaXPixel = x - breiteMitte;
	int deltaYPixel = y - hoeheMitte;

	//Lande in der Mitte des Pixels, statt auf den Ecken
	float deltaX = deltaXPixel * breiteProPixel + breiteProPixel / 2;
	float deltaY = -deltaYPixel * hoeheProPixel + breiteProPixel / 2; // Mit positivem deltaYPixel steht das Bild auf dem Kopf

	Vector AugpunktZuPunkt(deltaX, deltaY, this->distanzZuEbene);
	Vector normalisiert = AugpunktZuPunkt.normalize();

	return normalisiert;
}

Vector Camera::Position() const
{
	return Vector(0.0f, 0.0f, zwert); //Kamera ist stehts in positiver Richtung auf Z-Achse ausgerichtet, zwert wird negativ übergeben
}

SimpleRayTracer::SimpleRayTracer(unsigned int MaxDepth)
{
	m_MaxDepth = MaxDepth;
}

Color SimpleRayTracer::localIllumination(const Vector& Surface, const Vector& Eye, const Vector& N, const PointLight& Light, const Material& Mtrl)
{
	/*
	-Surface: Punkt auf der Oberfläche (Schnittpunkt Oberfläche mit Strahl),
	-Eye: Position des Augpunkts,
	-Normal: Normale des Dreiecks (übergeben Sie hier Triangle::calcNormal(...)),
	-Light: Eine Punktlichtquelle (Position und Intensität der Lichtquelle),
	-Material: das Material des Dreiecks
	*/

	Vector tmpN = N;
	Vector l = -(Surface - Light.Position);
	tmpN.normalize();

	Color D = Light.Intensity * Mtrl.getDiffuseCoeff(Surface) * std::max(0.0f, tmpN.dot(l.normalize()));

	Vector tmpE = -Eye;
	tmpE.normalize();

	Vector r = (tmpN*2 * tmpN.dot(l))-l;
	Color S = Light.Intensity * Mtrl.getSpecularCoeff(Surface) * pow(std::max(0.0f, tmpE.dot(r)), Mtrl.getSpecularExp(Surface));

	return D + S;
}

void SimpleRayTracer::traceScene(const Scene& SceneModel, RGBImage& Image)
{
	//Initialisiere Camera
	Camera cam(-8, 1, 1, 0.75, 640, 480);
	for (int x = 0; x < Image.width(); x++) //Iteriere über die Pixelmatrix mit doppelter for-Schleife
	{
		for (int y = 0; y < Image.height(); y++) 
		{
			Image.setPixelColor(x, y, Color(0, 0, 0));	//Färbe Pixel an Position x,y schwarz
			Vector s = cam.generateRay(x, y);			//Erzeuge Strahl von Augpunkt zu Pixel x,y
			Color F;
			F = trace(SceneModel, cam.Position(), s, this->m_MaxDepth);	//Rekursiver Aufruf des Strahlverfolgungsalgorithmus
			Image.setPixelColor(x, y, F);
		}
	}
}

Color SimpleRayTracer::trace(const Scene& SceneModel, const Vector& o, const Vector& d, int depth)
{
	Color c;

	//Wenn depth > 0, rufe trace erneut auf
	if (depth > 0) 
	{
		float distanz = std::numeric_limits<float>::max(); 
		int naechstesDreieck = -1;
		float tmpS;
		//iteriere über alle Dreiecke in der Szene
		for (int i = 0; i < SceneModel.getTriangleCount(); i++) 
		{
			//Wenn eine Kollision vom Augpunkt auf ein Dreieck erfolgt, speichere das Dreieck in naechstesDreieck und 
			//setze die Distanz auf die Strecke bis zum gefundenen Dreieck
			if (o.triangleIntersection(d, SceneModel.getTriangle(i).A, SceneModel.getTriangle(i).B, SceneModel.getTriangle(i).C, tmpS)) 
			{
				//Speichere das Dreieck nur, wenn es näher am Augpunkt ist, als das vorherige
				if (tmpS < distanz) {
					naechstesDreieck = i;
					distanz = tmpS;
				}
				
			}
		}

		if (naechstesDreieck != -1) 
		{
			const Triangle& triangle = SceneModel.getTriangle(naechstesDreieck);
			Vector auftreffpunkt = o + d * distanz;
			
			//SCHATTENSCHLAG
			//Prüfen, ob Lichtquellen für treffpunkt sichtbar sind
			//Iteriere über jede Lichtquelle der Szene
			for (int i = 0; i < SceneModel.getLightCount(); i++) 
			{
				const PointLight& light = SceneModel.getLight(i);
				Vector punktZumLicht = (auftreffpunkt - light.Position).normalize();
				int lichtDreieck = -1;
				float distanzLicht = std::numeric_limits<float>::max();

				//Für jede Lichtquelle, prüfe jedes Dreieck in der Szene, ob es vom Strahl einer Lichtquelle getroffen wird
				for (int j = 0; j < SceneModel.getTriangleCount(); j++)
				{
					//Suche das nächste Dreieck zum Licht
					if (light.Position.triangleIntersection(punktZumLicht, SceneModel.getTriangle(j).A, SceneModel.getTriangle(j).B, SceneModel.getTriangle(j).C, tmpS))
					{
						if (tmpS < distanzLicht) {
							lichtDreieck = j;
							distanzLicht = tmpS;
						}

					}
				}

				//Wenn das bestimmte Lichtdreieck das nächsteDreieck ist, dann wird die Fläche vom Licht getroffen und 
				//durch localillumination aufgehellt, sonst ist das Dreieck von einem anderen verdeckt
				if (lichtDreieck == naechstesDreieck ){
					c += localIllumination(auftreffpunkt, d, triangle.calcNormal(auftreffpunkt), light, *triangle.pMtrl);
				}
			}
			Vector r = d.reflection(triangle.calcNormal(auftreffpunkt));
			c += triangle.pMtrl->getAmbientCoeff(auftreffpunkt); //Ambienten Koeffizienten draufrechnen

			c += trace(SceneModel, auftreffpunkt, r, depth - 1) * triangle.pMtrl->getReflectivity(auftreffpunkt);
		}
	}
	return c;
}
