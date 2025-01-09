from django.views.generic import ListView, DetailView
from django.views.generic.edit import UpdateView
from django.urls import reverse_lazy, reverse

from blog.models import Publications


class PublicationsDetailView(DetailView):
    model = Publications

    def get_object(self, queryset=None):
        self.object = super().get_object(queryset)
        self.object.views += 1
        self.object.save()
        return self.object


class PublicationsListView(ListView):
    model = Publications

    def get_queryset(self):
        return Publications.objects.filter(publication=True)


class PublicationsUpdateView(UpdateView):
    model = Publications
    fields = ('title', 'description', 'image', 'publication', 'views')
    success_url = reverse_lazy('blog:publications_list')

    def get_success_url(self):
        return reverse('blog:publications_detail', args=[self.kwargs.get('pk')])
