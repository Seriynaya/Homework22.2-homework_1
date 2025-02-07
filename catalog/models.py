from django.db import models
from users.models import User
class Product(models.Model):

    name = models.CharField(max_length=150, verbose_name='наименование', help_text='Введите наименование')
    description = models.TextField(max_length=250, verbose_name='описание', help_text='Введите описание')
    image = models.ImageField(upload_to='images/products', blank=True, null=True, verbose_name='Изображение',
                              help_text='Загрузите изображение')
    category = models.ForeignKey("Category", on_delete=models.CASCADE, verbose_name='категория',
                                 help_text='Введите категорию', blank=True, null=True)
    price = models.IntegerField(verbose_name='Цена', help_text='Введите цену', blank=True, null=True)
    created_at = models.DateField(verbose_name='дата создания', help_text='Введите дату создания', blank=True,
                                  null=True)
    updated_at = models.DateTimeField(auto_now=True, verbose_name='дата последнего изменения', blank=True, null=True)

    owner = models.ForeignKey(User, verbose_name="Владелец", on_delete=models.CASCADE, blank=True, null=True)

    def __str__(self):
        return f'{self.name}'

    class Meta:
        verbose_name = 'товар'
        verbose_name_plural = 'товары'


class Category(models.Model):
    name = models.CharField(max_length=150, verbose_name='название', help_text='Введите название', blank=True,
                            null=True)
    description = models.TextField(max_length=250, verbose_name='описание', help_text='Введите описание', blank=True,
                                   null=True)

    def __str__(self):
        return f'{self.name}'

    class Meta:
        verbose_name = 'категория'
        verbose_name_plural = 'категории'