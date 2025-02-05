from django.db import models


class Publications(models.Model):
    title = models.CharField(max_length=150, verbose_name="Название статьи")
    description = models.TextField(verbose_name="Описание статьи", null=True, blank=True)
    image = models.ImageField(verbose_name="Превью статьи", blank=True)
    created_at = models.DateTimeField(auto_now_add=True, verbose_name='дата создания', blank=True)
    publication = models.BooleanField(verbose_name="Признак публикации", default=True)
    views = models.IntegerField(verbose_name="Количество просмотров", default=0)

    def __str__(self):
        return f'{self.title} {self.description}'

    class Meta:
        verbose_name = 'Статья'
        verbose_name_plural = 'Статьи'
        ordering = ['title', 'description']