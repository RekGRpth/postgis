/*
 * PostGIS Raster - Raster Types for PostGIS
 * http://trac.osgeo.org/postgis/wiki/WKTRaster
 *
 * Copyright (C) 2012 Regents of the University of California
 *   <bkpark@ucdavis.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "CUnit/Basic.h"
#include "cu_tester.h"

static void test_gdal_configured() {
	CU_ASSERT(rt_util_gdal_configured());
}

static void test_gdal_drivers() {
	uint32_t i;
	uint32_t size;
	rt_gdaldriver drv = NULL;

	drv = (rt_gdaldriver) rt_raster_gdal_drivers(&size, 1);
	CU_ASSERT(drv != NULL);

	for (i = 0; i < size; i++) {
		CU_ASSERT(strlen(drv[i].short_name));
		rtdealloc(drv[i].short_name);
		rtdealloc(drv[i].long_name);
		rtdealloc(drv[i].create_options);
	}

	rtdealloc(drv);
}

static void test_gdal_rasterize() {
	rt_raster raster;
	char srs[] = "PROJCS[\"unnamed\",GEOGCS[\"unnamed ellipse\",DATUM[\"unknown\",SPHEROID[\"unnamed\",6370997,0]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Lambert_Azimuthal_Equal_Area\"],PARAMETER[\"latitude_of_center\",45],PARAMETER[\"longitude_of_center\",-100],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1],AUTHORITY[\"EPSG\",\"2163\"]]";
	const char wkb_hex[] = "010300000001000000050000000000000080841ec100000000600122410000000080841ec100000000804f22410000000040e81dc100000000804f22410000000040e81dc100000000600122410000000080841ec10000000060012241";
	const char *pos = wkb_hex;
	unsigned char *wkb = NULL;
	int wkb_len = 0;
	int i;
	double scale_x = 100;
	double scale_y = -100;

	rt_pixtype pixtype[] = {PT_8BUI};
	double init[] = {0};
	double value[] = {1};
	double nodata[] = {0};
	uint8_t nodata_mask[] = {1};

	/* hex to byte */
	wkb_len = (int) ceil(((double) strlen(wkb_hex)) / 2);
	wkb = (unsigned char *) rtalloc(sizeof(unsigned char) * wkb_len);
	for (i = 0; i < wkb_len; i++) {
		int b = 0;
		sscanf(pos, "%2x", &b);
		wkb[i] = (unsigned char)b;
		pos += 2;
	}

	raster = rt_raster_gdal_rasterize(
		wkb,
		wkb_len, srs,
		1, pixtype,
		init, value,
		nodata, nodata_mask,
		NULL, NULL,
		&scale_x, &scale_y,
		NULL, NULL,
		NULL, NULL,
		NULL, NULL,
		NULL
	);

	CU_ASSERT(raster != NULL);
	CU_ASSERT_EQUAL(rt_raster_get_width(raster), 100);
	CU_ASSERT_EQUAL(rt_raster_get_height(raster), 100);
	CU_ASSERT_NOT_EQUAL(rt_raster_get_num_bands(raster), 0);
	CU_ASSERT_DOUBLE_EQUAL(rt_raster_get_x_offset(raster), -500000, DBL_EPSILON);
	CU_ASSERT_DOUBLE_EQUAL(rt_raster_get_y_offset(raster), 600000, DBL_EPSILON);

	rtdealloc(wkb);
	cu_free_raster(raster);
}

static rt_raster fillRasterToPolygonize(int hasnodata, double nodataval) {
	rt_band band = NULL;
	rt_pixtype pixtype = PT_32BF;

	/* Create raster */
	uint16_t width = 9;
	uint16_t height = 9;

	rt_raster raster = rt_raster_new(width, height);
	rt_raster_set_scale(raster, 1, 1);

	band = cu_add_band(raster, pixtype, hasnodata, nodataval);
	CU_ASSERT(band != NULL);

	{
		int x, y;
		for (x = 0; x < rt_band_get_width(band); ++x)
			for (y = 0; y < rt_band_get_height(band); ++y)
				rt_band_set_pixel(band, x, y, 0.0, NULL);
	}

	rt_band_set_pixel(band, 3, 1, 1.8, NULL);
	rt_band_set_pixel(band, 4, 1, 1.8, NULL);
	rt_band_set_pixel(band, 5, 1, 2.8, NULL);
	rt_band_set_pixel(band, 2, 2, 1.8, NULL);
	rt_band_set_pixel(band, 3, 2, 1.8, NULL);
	rt_band_set_pixel(band, 4, 2, 1.8, NULL);
	rt_band_set_pixel(band, 5, 2, 2.8, NULL);
	rt_band_set_pixel(band, 6, 2, 2.8, NULL);
	rt_band_set_pixel(band, 1, 3, 1.8, NULL);
	rt_band_set_pixel(band, 2, 3, 1.8, NULL);
	rt_band_set_pixel(band, 6, 3, 2.8, NULL);
	rt_band_set_pixel(band, 7, 3, 2.8, NULL);
	rt_band_set_pixel(band, 1, 4, 1.8, NULL);
	rt_band_set_pixel(band, 2, 4, 1.8, NULL);
	rt_band_set_pixel(band, 6, 4, 2.8, NULL);
	rt_band_set_pixel(band, 7, 4, 2.8, NULL);
	rt_band_set_pixel(band, 1, 5, 1.8, NULL);
	rt_band_set_pixel(band, 2, 5, 1.8, NULL);
	rt_band_set_pixel(band, 6, 5, 2.8, NULL);
	rt_band_set_pixel(band, 7, 5, 2.8, NULL);
	rt_band_set_pixel(band, 2, 6, 1.8, NULL);
	rt_band_set_pixel(band, 3, 6, 1.8, NULL);
	rt_band_set_pixel(band, 4, 6, 1.8, NULL);
	rt_band_set_pixel(band, 5, 6, 2.8, NULL);
	rt_band_set_pixel(band, 6, 6, 2.8, NULL);
	rt_band_set_pixel(band, 3, 7, 1.8, NULL);
	rt_band_set_pixel(band, 4, 7, 1.8, NULL);
	rt_band_set_pixel(band, 5, 7, 2.8, NULL);

	return raster;
}

static void test_gdal_polygonize() {
	int i;
	rt_raster rt;
	int nPols = 0;
	double total_area = 0;
	double total_val = 0;
	rt_geomval gv = NULL;
	LWGEOM *gobserved;
	//char *wkt = NULL;

	rt = fillRasterToPolygonize(1, -1.0);
	CU_ASSERT(rt_raster_has_band(rt, 0));

	nPols = 0;
	gv = rt_raster_gdal_polygonize(rt, 0, TRUE, &nPols);
	CU_ASSERT_DOUBLE_EQUAL(nPols, 4, FLT_EPSILON);
		total_area = 0; total_val = 0;
	for (i = 0; i < nPols; i++) {
		total_val += gv[i].val;
		gobserved = (LWGEOM *) gv[i].geom;
		total_area += lwgeom_area(gobserved);
		lwgeom_free((LWGEOM *) gv[i].geom);
	}
	printf("total area, total val, nPols  = %f, %f, %i\n", total_area, total_val, nPols);
	CU_ASSERT_DOUBLE_EQUAL(total_val, 1.8 + 0.0 + 2.8 + 0, FLT_EPSILON);
	CU_ASSERT_DOUBLE_EQUAL(total_area, 81, FLT_EPSILON);

	rtdealloc(gv);
	cu_free_raster(rt);

	/* Second test: NODATA value = 1.8 */
	rt = fillRasterToPolygonize(1, 1.8);

	/* We can check rt_raster_has_band here too */
	CU_ASSERT(rt_raster_has_band(rt, 0));

	nPols = 0;
	gv = rt_raster_gdal_polygonize(rt, 0, TRUE, &nPols);
	CU_ASSERT_DOUBLE_EQUAL(nPols, 4, FLT_EPSILON);
	total_area = 0; total_val = 0;
	for (i = 0; i < nPols; i++) {
		total_val += gv[i].val;
		gobserved = (LWGEOM *) gv[i].geom;
		total_area += lwgeom_area(gobserved);
		lwgeom_free((LWGEOM *) gv[i].geom);
	}
	printf("total area, total_val, polys = %f, %f, %i\n", total_area, total_val, nPols);
	CU_ASSERT_DOUBLE_EQUAL(total_val, 4.6, FLT_EPSILON);
	CU_ASSERT_DOUBLE_EQUAL(total_area, 81, FLT_EPSILON);


	rtdealloc(gv);
	cu_free_raster(rt);

	/* Third test: NODATA value = 2.8 */
	rt = fillRasterToPolygonize(1, 2.8);

	/* We can check rt_raster_has_band here too */
	CU_ASSERT(rt_raster_has_band(rt, 0));

	nPols = 0;
	gv = rt_raster_gdal_polygonize(rt, 0, TRUE, &nPols);
	CU_ASSERT_DOUBLE_EQUAL(nPols, 4, FLT_EPSILON);
	total_area = 0; total_val = 0;
	for (i = 0; i < nPols; i++) {
		total_val += gv[i].val;
		gobserved = (LWGEOM *) gv[i].geom;
		total_area += lwgeom_area(gobserved);
		lwgeom_free((LWGEOM *) gv[i].geom);
	}

	printf("total area, total_val, polys = %f, %f, %i\n", total_area, total_val, nPols);
	CU_ASSERT_DOUBLE_EQUAL(total_val, 4.6, FLT_EPSILON);
	CU_ASSERT_DOUBLE_EQUAL(total_area, 81, FLT_EPSILON);

	rtdealloc(gv);
	cu_free_raster(rt);

	/* Fourth test: NODATA value = 0 */
	rt = fillRasterToPolygonize(1, 0.0);
	/* We can check rt_raster_has_band here too */
	CU_ASSERT(rt_raster_has_band(rt, 0));

	nPols = 0;
	gv = rt_raster_gdal_polygonize(rt, 0, TRUE, &nPols);

	CU_ASSERT_DOUBLE_EQUAL(nPols, 2, FLT_EPSILON);
	total_area = 0; total_val = 0;
	for (i = 0; i < nPols; i++) {
		total_val += gv[i].val;
		gobserved = (LWGEOM *) gv[i].geom;
		total_area += lwgeom_area(gobserved);
		lwgeom_free((LWGEOM *) gv[i].geom);
	}

	printf("total area, total_val, polys = %f, %f, %i\n", total_area, total_val, nPols);
	CU_ASSERT_DOUBLE_EQUAL(total_val, 4.6, FLT_EPSILON);
	CU_ASSERT_DOUBLE_EQUAL(total_area, 28, FLT_EPSILON);

	rtdealloc(gv);
	cu_free_raster(rt);

	/* Last test: There is no NODATA value (all values are valid) */
	rt = fillRasterToPolygonize(0, 0.0);
	/* We can check rt_raster_has_band here too */
	CU_ASSERT(rt_raster_has_band(rt, 0));

	nPols = 0;
	gv = rt_raster_gdal_polygonize(rt, 0, TRUE, &nPols);

	CU_ASSERT_DOUBLE_EQUAL(nPols, 4, FLT_EPSILON);
	total_area = 0; total_val = 0;
	for (i = 0; i < nPols; i++) {
		total_val += gv[i].val;
		gobserved = (LWGEOM *) gv[i].geom;
		total_area += lwgeom_area(gobserved);
		lwgeom_free((LWGEOM *) gv[i].geom);
	}

	printf("total area, total_val, polys = %f, %f, %i\n", total_area, total_val, nPols);
	CU_ASSERT_DOUBLE_EQUAL(total_val, 1.8 + 0.0 + 2.8 + 0.0, FLT_EPSILON);
	CU_ASSERT_DOUBLE_EQUAL(total_area, 81, FLT_EPSILON);
	rtdealloc(gv);
	cu_free_raster(rt);
}

static void test_raster_to_gdal() {
	rt_pixtype pixtype = PT_64BF;
	rt_raster raster = NULL;
	rt_band band = NULL;
	uint32_t x;
	uint32_t width = 100;
	uint32_t y;
	uint32_t height = 100;
	char srs[] = "PROJCS[\"unnamed\",GEOGCS[\"unnamed ellipse\",DATUM[\"unknown\",SPHEROID[\"unnamed\",6370997,0]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Lambert_Azimuthal_Equal_Area\"],PARAMETER[\"latitude_of_center\",45],PARAMETER[\"longitude_of_center\",-100],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1],AUTHORITY[\"EPSG\",\"2163\"]]";

	uint64_t gdalSize;
	uint8_t *gdal = NULL;

	raster = rt_raster_new(width, height);
	CU_ASSERT(raster != NULL); /* or we're out of virtual memory */

	band = cu_add_band(raster, pixtype, 1, 0);
	CU_ASSERT(band != NULL);

	rt_raster_set_offsets(raster, -500000, 600000);
	rt_raster_set_scale(raster, 1000, -1000);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			rt_band_set_pixel(band, x, y, (((double) x * y) + (x + y) + (x + y * x)) / (x + y + 1), NULL);
		}
	}

	gdal = rt_raster_to_gdal(raster, srs, "GTiff", NULL, &gdalSize);
	/*printf("gdalSize: %d\n", (int) gdalSize);*/
	CU_ASSERT(gdalSize);

	/*
	FILE *fh = NULL;
	fh = fopen("/tmp/out.tif", "w");
	fwrite(gdal, sizeof(uint8_t), gdalSize, fh);
	fclose(fh);
	*/

	if (gdal) CPLFree(gdal);

	cu_free_raster(raster);

	raster = rt_raster_new(width, height);
	CU_ASSERT(raster != NULL); /* or we're out of virtual memory */

	band = cu_add_band(raster, pixtype, 1, 0);
	CU_ASSERT(band != NULL);

	rt_raster_set_offsets(raster, -500000, 600000);
	rt_raster_set_scale(raster, 1000, -1000);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			rt_band_set_pixel(band, x, y, x, NULL);
		}
	}

	/* add check that band isn't NODATA */
	CU_ASSERT_EQUAL(rt_band_check_is_nodata(band), FALSE);

	gdal = rt_raster_to_gdal(raster, srs, "PNG", NULL, &gdalSize);
	/*printf("gdalSize: %d\n", (int) gdalSize);*/
	CU_ASSERT(gdalSize);

	if (gdal) CPLFree(gdal);

	gdal = rt_raster_to_gdal(raster, srs, "PCIDSK", NULL, &gdalSize);
	CU_ASSERT(gdal == NULL);

	cu_free_raster(raster);
}

static void test_gdal_to_raster() {
	rt_pixtype pixtype = PT_64BF;
	rt_band band = NULL;

	rt_raster raster;
	rt_raster rast;
	const uint32_t width = 100;
	const uint32_t height = 100;
	uint32_t x;
	uint32_t y;
	int v;
	double values[width][height];
	int rtn = 0;
	double value;

	GDALDriverH gddrv = NULL;
	int destroy = 0;
	GDALDatasetH gdds = NULL;

	raster = rt_raster_new(width, height);
	CU_ASSERT(raster != NULL); /* or we're out of virtual memory */

	band = cu_add_band(raster, pixtype, 1, 0);
	CU_ASSERT(band != NULL);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			values[x][y] = (((double) x * y) + (x + y) + (x + y * x)) / (x + y + 1);
			rt_band_set_pixel(band, x, y, values[x][y], NULL);
		}
	}

	gdds = rt_raster_to_gdal_mem(raster, NULL, NULL, NULL, 0, &gddrv, &destroy);
	CU_ASSERT(gddrv != NULL);
	CU_ASSERT(gdds != NULL);
	CU_ASSERT_EQUAL((uint32_t)GDALGetRasterXSize(gdds), width);
	CU_ASSERT_EQUAL((uint32_t)GDALGetRasterYSize(gdds), height);

	rast = rt_raster_from_gdal_dataset(gdds);
	CU_ASSERT(rast != NULL);
	CU_ASSERT_EQUAL(rt_raster_get_num_bands(rast), 1);

	band = rt_raster_get_band(rast, 0);
	CU_ASSERT(band != NULL);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			rtn = rt_band_get_pixel(band, x, y, &value, NULL);
 			CU_ASSERT_EQUAL(rtn, ES_NONE);
			CU_ASSERT_DOUBLE_EQUAL(value, values[x][y], DBL_EPSILON);
		}
	}

	if (destroy && gddrv) {
		GDALDeregisterDriver(gddrv);
		GDALDestroyDriver(gddrv);
	}
	GDALClose(gdds);
	gdds = NULL;
	gddrv = NULL;

	cu_free_raster(rast);
	cu_free_raster(raster);

	raster = rt_raster_new(width, height);
	CU_ASSERT(raster != NULL); /* or we're out of virtual memory */

	pixtype = PT_8BSI;
	band = cu_add_band(raster, pixtype, 1, 0);
	CU_ASSERT(band != NULL);

	v = -127;
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			values[x][y] = v++;
			rt_band_set_pixel(band, x, y, values[x][y], NULL);
			if (v == 128)
				v = -127;
		}
	}

	gdds = rt_raster_to_gdal_mem(raster, NULL, NULL, NULL, 0, &gddrv, &destroy);
	CU_ASSERT(gddrv != NULL);
	CU_ASSERT(gdds != NULL);
	CU_ASSERT_EQUAL((uint32_t)GDALGetRasterXSize(gdds), width);
	CU_ASSERT_EQUAL((uint32_t)GDALGetRasterYSize(gdds), height);

	rast = rt_raster_from_gdal_dataset(gdds);
	CU_ASSERT(rast != NULL);
	CU_ASSERT_EQUAL(rt_raster_get_num_bands(rast), 1);

	band = rt_raster_get_band(rast, 0);
	CU_ASSERT(band != NULL);
#if POSTGIS_GDAL_VERSION < 30700
	CU_ASSERT_EQUAL(rt_band_get_pixtype(band), PT_16BSI);
#else
	CU_ASSERT_EQUAL(rt_band_get_pixtype(band), PT_8BSI);
#endif
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			rtn = rt_band_get_pixel(band, x, y, &value, NULL);
#if POSTGIS_GDAL_VERSION < 30700
			CU_ASSERT_DOUBLE_EQUAL(value, values[x][y], 1.);
			CU_ASSERT_DOUBLE_EQUAL(value, values[x][y], DBL_EPSILON);
#else
 			CU_ASSERT_EQUAL(rtn, ES_NONE);
			CU_ASSERT_DOUBLE_EQUAL(value, values[x][y], DBL_EPSILON);
#endif
		}
	}
	if (destroy && gddrv) {
		GDALDeregisterDriver(gddrv);
		GDALDestroyDriver(gddrv);
	}

	GDALClose(gdds);
	gdds = NULL;
	gddrv = NULL;

	cu_free_raster(rast);
	cu_free_raster(raster);
}

static void test_gdal_warp() {
	rt_pixtype pixtype = PT_64BF;
	rt_band band = NULL;

	rt_raster raster;
	rt_raster rast;
	uint32_t x;
	uint32_t width = 100;
	uint32_t y;
	uint32_t height = 100;
	double value = 0;

	char src_srs[] = "PROJCS[\"unnamed\",GEOGCS[\"unnamed ellipse\",DATUM[\"unknown\",SPHEROID[\"unnamed\",6370997,0]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Lambert_Azimuthal_Equal_Area\"],PARAMETER[\"latitude_of_center\",45],PARAMETER[\"longitude_of_center\",-100],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1],AUTHORITY[\"EPSG\",\"2163\"]]";

	char dst_srs[] = "PROJCS[\"NAD83 / California Albers\",GEOGCS[\"NAD83\",DATUM[\"North_American_Datum_1983\",SPHEROID[\"GRS 1980\",6378137,298.257222101,AUTHORITY[\"EPSG\",\"7019\"]],AUTHORITY[\"EPSG\",\"6269\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4269\"]],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],PROJECTION[\"Albers_Conic_Equal_Area\"],PARAMETER[\"standard_parallel_1\",34],PARAMETER[\"standard_parallel_2\",40.5],PARAMETER[\"latitude_of_center\",0],PARAMETER[\"longitude_of_center\",-120],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",-4000000],AUTHORITY[\"EPSG\",\"3310\"],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";

	raster = rt_raster_new(width, height);
	CU_ASSERT(raster != NULL); /* or we're out of virtual memory */

	band = cu_add_band(raster, pixtype, 1, 0);
	CU_ASSERT(band != NULL);

	rt_raster_set_offsets(raster, -500000, 600000);
	rt_raster_set_scale(raster, 1000, -1000);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			rt_band_set_pixel(band, x, y, (((double) x * y) + (x + y) + (x + y * x)) / (x + y + 1), NULL);
		}
	}

	rast = rt_raster_gdal_warp(
		raster,
		src_srs, dst_srs,
		NULL, NULL,
		NULL, NULL,
		NULL, NULL,
		NULL, NULL,
		NULL, NULL,
		GRA_NearestNeighbour, -1
	);
	CU_ASSERT(rast != NULL);
	CU_ASSERT_EQUAL(rt_raster_get_width(rast), 122);
	CU_ASSERT_EQUAL(rt_raster_get_height(rast), 116);
	CU_ASSERT_NOT_EQUAL(rt_raster_get_num_bands(rast), 0);

	band = rt_raster_get_band(rast, 0);
	CU_ASSERT(band != NULL);

	CU_ASSERT(rt_band_get_hasnodata_flag(band));
	rt_band_get_nodata(band, &value);
	CU_ASSERT_DOUBLE_EQUAL(value, 0., DBL_EPSILON);

	CU_ASSERT_EQUAL(rt_band_get_pixel(band, 0, 0, &value, NULL), ES_NONE);
	CU_ASSERT_DOUBLE_EQUAL(value, 0., DBL_EPSILON);

	cu_free_raster(rast);
	cu_free_raster(raster);
}

static void test_gdal_warp_preserves_data(void) {
	const char *filename = POSTGIS_TOP_SRC_DIR "/raster/test/regress/loader/Projected.tif";

	GDALDatasetH hDS_in = NULL;
	rt_raster rast_in = NULL;
	rt_raster rast_out = NULL;
	int band_count_in, band_count_out, i;

	// double scale_x = 0.0, scale_y = 0.0;
	// double dim_x = 0.0, dim_y = 0.0;
	// int width = 0, height = 0;
	// double grid_xw = 0.0, grid_yw = 0.0;
	// double skew_x = 0.0, skew_y = 0.0;

	double max_err = 0.125;
	GDALResampleAlg alg = GRA_NearestNeighbour;

	const char *src_srs = "EPSG:4326";
	const char *dst_srs = "EPSG:3857";

	/* Handle to TIFF */
	GDALAllRegister();
	hDS_in = GDALOpen(filename, GA_ReadOnly);
	CU_ASSERT(hDS_in != NULL);

	/* Read TIFF into memory as rt_raster */
	rast_in = rt_raster_from_gdal_dataset(hDS_in);
	CU_ASSERT(rast_in != NULL);

	/* Warp raster using default options */
	rast_out = rt_raster_gdal_warp(rast_in,
	    src_srs, dst_srs,
	    NULL, NULL, // &scale_x, &scale_y,
	    NULL, NULL, // &dim_x, &dim_y,
	    NULL, NULL, // &width, &height,
	    NULL, NULL, // &grid_xw, &grid_yw,
	    NULL, NULL, // &skew_x, &skew_y,
	    alg, max_err);
	CU_ASSERT(rast_out != NULL);

	band_count_in = rt_raster_get_num_bands(rast_in);
	band_count_out = rt_raster_get_num_bands(rast_out);
	CU_ASSERT_EQUAL(band_count_in, band_count_out);

	for (i = 0; i < band_count_in; i++) {
		double tolerance = 0.1;
		rt_bandstats stats_in, stats_out;
		rt_band band_in = rt_raster_get_band(rast_in, i);
		rt_band band_out = rt_raster_get_band(rast_out, i);

		CU_ASSERT(band_in != NULL);
		CU_ASSERT(band_out != NULL);

		stats_in = rt_band_get_summary_stats(band_in, 1, 1, 0, NULL, NULL, NULL);
		stats_out = rt_band_get_summary_stats(band_out, 1, 1, 0, NULL, NULL, NULL);

		CU_ASSERT_DOUBLE_EQUAL(stats_in->min, stats_out->min, fabs(stats_in->min) * tolerance);
		CU_ASSERT_DOUBLE_EQUAL(stats_in->max, stats_out->max, fabs(stats_in->max) * tolerance);
		CU_ASSERT_DOUBLE_EQUAL(stats_in->mean, stats_out->mean, fabs(stats_in->mean) * tolerance);
	}

	rt_raster_destroy(rast_in);
	rt_raster_destroy(rast_out);
	GDALClose(hDS_in);
}

/* register tests */
void gdal_suite_setup(void);
void gdal_suite_setup(void)
{
	CU_pSuite suite = CU_add_suite("gdal", NULL, NULL);
	PG_ADD_TEST(suite, test_gdal_configured);
	PG_ADD_TEST(suite, test_gdal_drivers);
	PG_ADD_TEST(suite, test_gdal_rasterize);
	PG_ADD_TEST(suite, test_gdal_polygonize);
	PG_ADD_TEST(suite, test_raster_to_gdal);
	PG_ADD_TEST(suite, test_gdal_to_raster);
	PG_ADD_TEST(suite, test_gdal_warp);
	PG_ADD_TEST(suite, test_gdal_warp_preserves_data);
}

