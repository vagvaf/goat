"""added system table

Revision ID: d0d7dbd36161
Revises: c82e304d9689
Create Date: 2022-09-10 17:13:48.265210

"""
from alembic import op
import sqlalchemy as sa
import geoalchemy2
import sqlmodel  

from sqlalchemy.dialects import postgresql

# revision identifiers, used by Alembic.
revision = 'd0d7dbd36161'
down_revision = 'c82e304d9689'
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.create_table('system',
    sa.Column('id', sa.Integer(), autoincrement=True, nullable=False),
    sa.Column('type', sa.Text(), nullable=False),
    sa.Column('setting', postgresql.JSONB(astext_type=sa.Text()), nullable=False),
    sa.PrimaryKeyConstraint('id'),
    schema='customer'
    )
    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.drop_table('system', schema='customer')
    # ### end Alembic commands ###
