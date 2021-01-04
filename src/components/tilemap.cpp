#include "tilemap.h"

using namespace TL;

Tilemap::Tilemap()
{
}

Tilemap::Tilemap(int tile_width, int tile_height, int columns, int rows)
{
	m_tile_width = tile_width;
	m_tile_height = tile_height;
	m_columns = columns;
	m_rows = rows;
	m_grid.resize(columns * rows);
}

int Tilemap::tile_width() const
{
	return m_tile_width;
}

int Tilemap::tile_height() const
{
	return m_tile_height;
}

int Tilemap::columns() const
{
	return m_columns;
}

int Tilemap::rows() const
{
	return m_rows;
}

void Tilemap::set_cell(int x, int y, const Subtexture* tex)
{
	if (tex)
		m_grid[x + y * m_columns] = *tex;
	else
		m_grid[x + y * m_columns].texture.reset();
}

void Tilemap::set_cells(int x, int y, int w, int h, const Subtexture* tex)
{
	for (int tx = x; tx < x + w; tx++)
		for (int ty = y; ty < y + h; ty++)
			set_cell(tx, ty, tex);
}

void Tilemap::render(Batch& batch)
{
	batch.push_matrix(Mat3x2::create_translation(entity()->position));
	for (int x = 0; x < m_columns; x ++)
		for (int y = 0; y < m_rows; y ++)
			if (m_grid[x + y * m_columns].texture)
			{
				batch.tex(m_grid[x + y * m_columns], Vec2(x * m_tile_width, y * m_tile_height));
			}
	batch.pop_matrix();
}
