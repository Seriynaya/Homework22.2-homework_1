from django.contrib import admin
from .models import Publications


@admin.register(Publications)
class PublicationsAdmin(admin.ModelAdmin):
    list_display = ('id', 'title', 'description', 'views')
    list_filter = ('publication',)
    search_fields = ('title', 'description')