from django.forms import ModelForm
from .models import Category, Product
from django.core.exceptions import ValidationError


class ProductForm(ModelForm):
    class Meta:
        model = Product
        fields = '__all__'

    def __init__(self, *args, **kwargs):
        super(ProductForm, self).__init__(*args, **kwargs)
        self.fields['name'].widget.attrs.update({
            'class': 'form-control',
            'placeholder': 'Введите название продукта'
        })
        self.fields['description'].widget.attrs.update({'class': 'form-control', 'placeholder': 'Введите описание'})
        self.fields['image'].widget.attrs.update({'class': 'form-control'})
        self.fields['category'].widget.attrs.update({'class': 'form-control'})
        self.fields['price'].widget.attrs.update({'class': 'form-control', 'placeholder': 'Введите цену'})
        self.fields['created_at'].widget.attrs.update({'class': 'form-control', 'placeholder': 'Введите дату'})


    def clean_name(self):
        banwords = ["казино", "биржа", "обман", "криптовалюта", "дешево", "полиция", "крипта", "бесплатно", "радар"]
        name = self.cleaned_data.get('name')
        for banword in banwords:
            if banword in name.lower():
                raise ValidationError(f'Слово "{banword}" запрещено для имени товара')
        return name

    def clean_description(self):
        banwords = ["казино", "биржа", "обман", "криптовалюта", "дешево", "полиция", "крипта", "бесплатно", "радар"]
        description = self.cleaned_data.get('description')
        for banword in banwords:
            if banword in description.lower():
                raise ValidationError(f'Слово "{banword}" запрещено для описания товара')
        return description

    def clean_price(self):
        price = self.cleaned_data.get('price')
        if price < 0:
            raise ValidationError(f'Цена не должна быть отрицательной')
        return price


class CategoryForm(ModelForm):
    class Meta:
        model = Category
        fields = ['name', 'description']


class ProductModeratorForm(ModelForm):
    class Meta:
        model = Product
        fields = ['status_publication']