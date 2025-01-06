from django.core.management.base import BaseCommand
from catalog.models import Product, Category


class Command(BaseCommand):
    help = 'заполнения базы-данных о товарах'

    def handle(self, *args, **kwargs):
        Product.objects.all().delete()

        сategory, _ = Category.objects.get_or_create(name='Техника')

        products = [
            {'name': 'компьютер',"description": "игровой", 'сategory': сategory},
            {"name": 'ноутбук', "description": "игровой", 'сategory': сategory}
        ]

        for prod in products:
            product, created = Product.objects.get_or_create(**prod)
            if created:
                self.stdout.write(self.style.SUCCESS(f'Successfully added student: '
                                                     f'{product.name} {product.description}'))
            else:
                self.stdout.write(self.style.WARNING(f'Catalog already exists: 'f'{product.name} {product.description}'))