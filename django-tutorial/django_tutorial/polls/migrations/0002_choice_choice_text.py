# Generated by Django 5.0.4 on 2024-05-04 17:31

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('polls', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='choice',
            name='choice_text',
            field=models.CharField(default='', max_length=200),
            preserve_default=False,
        ),
    ]